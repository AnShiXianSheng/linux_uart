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
#include "can-msg.h"

#define rvm_debug(s, ...)          dbg_errfl(s, ##__VA_ARGS__)

#define RVM_SPI_PATH "/dev/spidev0.0"
#define RVM_UART_PATH "/dev/ttyLP1"
#define RVM_SPI_SPEED 10000000

static SpiRegHandle spiRegHandle;

static RegWrCbHandle regWrCbHandle = {
    .read_reg = &RVMcu_ReadReg,
    .write_reg = &RVMcu_WriteReg
};

static int _BurnMcu(const char* mcu_firmware_path){
#define BURN_WR_MAX_RETRY   5
#define BURN_PARTICLE_SIZE          512
    BurnStaReg reg = {0};
    uint8_t *start,*end;
    int firmware_fd;
    int r_len,retry;
    static uint8_t firmware_data[BURN_PARTICLE_SIZE];
    int ret;
    
    reg.burn_mode = BURNMODE_START_ERASE;
    ret = RVMcu_WriteReg(RWREG_BURN_START, (uint8_t*)&reg, sizeof(reg.burn_mode), 500);
    if(ret < 0){
        rvm_debug("启动擦除模式失败");
        return ret;
    }
    usleep(50000);
    ret = RVMcu_ReadReg(RWREG_BURN_START, (uint8_t *)&reg, sizeof(reg), 500);
    if(ret < 0){
        rvm_debug("读烧写寄存器失败 %d", ret);
        return ret;
    }
    if(reg.burn_mode != BURNMODE_ERASE_OK){
        rvm_debug("擦除失败... error code", reg.error_code);
        return -reg.error_code;
    }
    reg.burn_mode = BURNMODE_START_BURN;
    ret = RVMcu_WriteReg(RWREG_BURN_START, (uint8_t*)&reg, sizeof(reg.burn_mode), 500);
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
            if(ret > 0){
                start += ret;
                continue;
            }
            if(ret == 0){
                ret = RVMcu_ReadReg(RWREG_BURN_START, (uint8_t *)&reg, sizeof(reg), 500);
                if(ret < 0)
                    goto burn_out;
                if(reg.burn_mode == BURNMODE_BURN_ERROR){
                    ret = -reg.error_code;
                    rvm_debug("烧写失败 %d", ret);
                    goto burn_out;
                }
            }
            if(retry++ > BURN_WR_MAX_RETRY){
                rvm_debug("打开烧写错误");
                goto burn_out;
            }
        }
    }

    reg.burn_mode = BURNMODE_FINISH_TRANSFER;
    reg.error_code = 0;
    RVMcu_WriteReg(RWREG_BURN_START, (uint8_t*)&reg, sizeof(reg.burn_mode), 500);

    /* mcu 会校验结果 */
    while(1){
        ret = RVMcu_ReadReg(RWREG_BURN_START, (uint8_t *)&reg, sizeof(reg), 500);
        if(ret < 0){
            rvm_debug("读烧写寄存器失败 %d", ret);
            goto burn_out;
        }
        if(reg.burn_mode != BURNMODE_FINISH_TRANSFER){
            //rvm_debug("烧写完成 %d", reg.error_code);
            ret = -reg.error_code;
            break;
        }
        usleep(2000);
    }

    if(ret < 0)
        goto burn_out;
    reg.burn_mode = BURNMODE_EXIT_BURN;
    RVMcu_WriteReg(RWREG_BURN_START, (uint8_t*)&reg, sizeof(reg.burn_mode), 200);
burn_out:
    close(firmware_fd);
    return ret;
}

int RVMcu_ReadReg(uint16_t reg_addr,  uint8_t *reg_data, uint16_t reg_cnt, uint32_t timeout){
    return SpiReg_Read(&spiRegHandle, reg_addr, reg_cnt, reg_data, timeout);
}

/**
 * @brief  写MCU寄存器
 * @param  reg_addr         寄存器地址
 * @param  reg_data         寄存器数据
 * @param  reg_cnt          寄存器数量
 * @param  timeout          通信超时时间
 * @return int 
 */
int RVMcu_WriteReg(uint16_t reg_addr, const uint8_t *reg_data, uint16_t reg_cnt, uint32_t timeout){
    return SpiReg_Write(&spiRegHandle, reg_addr, reg_cnt, reg_data, timeout);
}

/**
 * @brief 发送CAN报文
 * @param  can_msg          can报文结构体指针
 * @param  timeout          超时时间
 * @return int 
 */
int RVMcu_SendCanMsg(PCanMsg *can_msg, uint32_t timeout){
    return RegWrCb_Write(&regWrCbHandle, RWREG_CB_MPU_BUSINESS_SEND_CAN_START, (uint8_t*)can_msg, sizeof(PCanMsg), timeout);
}

/**
 * @brief 接收CAN报文
 * @param  can_msg          can报文结构体指针
 * @param  timeout          超时时间
 * @return int              成功返回1 无数据0 错误负数
 */
int RVMcu_ReceiveCanMsg(PCanMsg *can_msg, uint32_t timeout){
    int ret;
    ret = RegWrCb_Read(&regWrCbHandle, RWREG_CB_MPU_BUSINESS_RECEIVE_CAN_START, (uint8_t*)can_msg, sizeof(PCanMsg), timeout);
    if(ret > 0) return 1;
    return ret;
}

/**
 * @brief 接收多块CAN报文用这个效率会高很多
 * @param  can_msg          can报文结构体指针
 * @param  cnt              can_msg 数组的长度
 * @param  timeout          超时时间
 * @return int 
 */
int RVMcu_ReceiveCanMsgBlock(PCanMsg *can_msg, uint32_t cnt,  uint32_t timeout){
    int ret;
    ret = RegWrCb_Read(&regWrCbHandle, RWREG_CB_MPU_BUSINESS_RECEIVE_CAN_START, (uint8_t*)can_msg, sizeof(PCanMsg) * cnt, timeout);
    if(ret > 0)
        return ret/sizeof(PCanMsg);
    return ret;
}


/**
 * @brief 烧写MCU
 * @param  mcu_firmware_path    固件路径
 * @return int 
 */
int RVMcu_BurnMcu(const char* mcu_firmware_path){

    BurnStaReg reg = {0};
    McuInfo    info;
    int ret;

    ret = RVMcu_ReadReg(ROREG_INFO_START, (uint8_t *)&info, sizeof(info), 200);
    if(ret < 0){
        rvm_debug("读INFO寄存器失败 %d", ret);
        return ret;
    }

    if(info.partition != 0){
        /* 告诉APP进入BOOTLOADER模式 */
        reg.burn_mode = BURNMODE_APP_GOTO_BOOTLOADER;
        ret = RVMcu_WriteReg(RWREG_BURN_START, (uint8_t*)&reg, sizeof(reg.burn_mode), 200);
        if(ret < 0){
            rvm_debug("启动烧写模式失败");
            return ret;
        }
        usleep(100000);
    }

    ret = RVMcu_ReadReg(RWREG_BURN_START, (uint8_t *)&reg, sizeof(reg), 500);
    if(ret < 0){
        rvm_debug("读烧写寄存器失败 %d", ret);
        return ret;
    }
    if(reg.burn_mode != BURNMODE_WAIT_BURN){
        rvm_debug("MCU不在烧写模式或者忙");
        return -1;
    }
    return _BurnMcu(mcu_firmware_path);
}

/**
 * @brief  强行烧写MCU
 * @param  mcu_firmware_path    固件路径
 * @return int 
 */
int RVMcu_ForceBurnMcu(const char* mcu_firmware_path){

    BurnStaReg reg = {0};
    McuInfo    info;
    int ret;
    while(1){
        ret = RVMcu_ReadReg(ROREG_INFO_START, (uint8_t *)&info, sizeof(info), 10);
        if(ret == 0 && info.partition == 0 ) break;
    }

    reg.burn_mode = BURNMODE_START_ERASE;
    while(1){
        ret = RVMcu_WriteReg(RWREG_BURN_START, (uint8_t*)&reg, sizeof(reg.burn_mode), 10);
        if(ret == 0) break;
    }

    return _BurnMcu(mcu_firmware_path);
}


int RVMcu_Init(void){
    return SpiReg_Init(&spiRegHandle, RVM_SPI_PATH, RVM_UART_PATH, RVM_SPI_SPEED);
}

void RVMcu_Exit(void){
    SpiReg_Exit(&spiRegHandle);
}