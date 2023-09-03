/**
 * @file rearview_mcu.c
 * @brief 后视镜MCU数据接口
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-09-02
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "spi_reg.h"
#include "regwr_cb.h"
#include "debug.h"

#include "rearview_mcu.h"
#include "typedef.h"

#define rvm_debug(s, ...)          dbg_errfl(s, ##__VA_ARGS__)

#define RVM_SPI_PATH "/dev/spidev0.0"
#define RVM_UART_PATH "/dev/ttyLP1"
#define RVM_SPI_SPEED 10000000

static SpiRegHandle spiRegHandle;

static RegWrCbHandle regWrCbHandle = {
    .read_reg = &RVMcu_ReadReg,
    .write_reg = &RVMcu_WriteReg
};

int RVMcu_ReadReg(uint16_t reg_addr,  uint8_t *reg_data, uint16_t reg_cnt, uint32_t timeout){
    return SpiReg_Read(&spiRegHandle, reg_addr, reg_cnt, reg_data, timeout);
}

int RVMcu_WriteReg(uint16_t reg_addr, const uint8_t *reg_data, uint16_t reg_cnt, uint32_t timeout){
    return SpiReg_Write(&spiRegHandle, reg_addr, reg_cnt, reg_data, timeout);
}


/**
 * @brief 烧写MCU
 * @param  mcu_firmware_path    固件路径
 * @return int 
 */
int RVMcu_BurnMcu(const char* mcu_firmware_path){
#define BURN_WR_MAX_RETRY   10
#define BURN_PARTICLE_SIZE          512
    BurnStaReg reg = {0};
    uint8_t *start,*end;
    int firmware_fd;
    int r_len,retry;
    static uint8_t firmware_data[BURN_PARTICLE_SIZE];
    int ret;
    /* 假设已经到了bootloader */
    reg.is_goto_burn_mode = 1;
    
    ret = RVMcu_WriteReg(RWREG_BURN_START + offsetof(BurnStaReg, is_goto_burn_mode), 
            &reg.is_goto_burn_mode , sizeof(reg.is_goto_burn_mode), 200);
    if(ret < 0){
        rvm_debug("启动烧写模式失败");
        return ret;
    }

    RegWrCb_Clean(&regWrCbHandle, RWREG_CB_BURN_START,200);
    firmware_fd = open(mcu_firmware_path, O_RDONLY);
    if(firmware_fd < 0){
        rvm_debug("打开烧写文件失败: %d %s", firmware_fd, mcu_firmware_path);
        return firmware_fd;
    }

    while(1){
        r_len = read(firmware_fd, firmware_data, BURN_PARTICLE_SIZE);
        if(r_len <= 0)
            break;
        start = firmware_data;
        end = firmware_data + r_len;
        retry = 0;
        while(start < end){
            ret = RegWrCb_Write(&regWrCbHandle, RWREG_CB_BURN_START, start, end-start, 200);
            if(ret >= 0){
                start += ret;
                continue;
            }
            if(retry++ > BURN_WR_MAX_RETRY*2){
                rvm_debug("打开烧写错误");
                goto burn_error;
            }
        }
    }

    reg.is_goto_burn_mode = 2;
    reg.error_code = 0;
    ret = RVMcu_WriteReg(RWREG_BURN_START, (uint8_t*)&reg, sizeof(BurnStaReg), 200);
    if(ret < 0){
        rvm_debug("结束烧写时写寄存器失败");
        return ret;
    }
    ret = RVMcu_ReadReg(RWREG_BURN_START, (uint8_t*)&reg, sizeof(BurnStaReg), 200);
    if(ret < 0){
        rvm_debug("结束烧写时读寄存器失败");
        return ret;
    }
    if(reg.is_goto_burn_mode == 3 && reg.error_code == 0)
        return 0;
    rvm_debug("烧写失败: errcode:%d", reg.error_code);
    return -reg.error_code;
burn_error:
    close(firmware_fd);
    return ret;
}


int RVMcu_Init(void){
    return SpiReg_Init(&spiRegHandle, RVM_SPI_PATH, RVM_UART_PATH, RVM_SPI_SPEED);
}

void RVMcu_Exit(void){
     SpiReg_Exit(&spiRegHandle);
}