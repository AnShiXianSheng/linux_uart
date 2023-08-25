/**
 * @file run.c
 * @brief 
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-08-18
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "memctrl.h"
#include "spi_reg.h"
#include "run.h"
#include "debug.h"
#include "typedef.h"


#define ROREG_SN_SIZE   32

#pragma pack(1)
typedef struct _RoReg{
    struct {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
        uint8_t reserved;
    }software_version;
    char        sn[ROREG_SN_SIZE];
    uint32_t    partition;  /* 当前分区 0：bootloder 1:app */
}RoReg;

#pragma pack()


static void make_data(uint8_t* wr_buf, uint16_t cnt ){
    int i;
    srand(time(NULL));

    for(i=0;i<cnt;i++){
        wr_buf[i] = (uint8_t)rand();
    }
}

static int _run_test(RunConfig *config){
    int i;
    int err_cnt = 0;
    int ret;
    uint32_t start_time;
    start_time = GET_TICK();
    for(i=0;i<config->test_cnt;i++){
        
        if(config->is_write){
            make_data(config->wr_buf, config->reg_cnt);
            ret = SpiReg_Write(&config->spi_reg_handle, config->reg_addr, config->reg_cnt, config->wr_buf);
        }else{
            ret = SpiReg_Read(&config->spi_reg_handle, config->reg_addr, config->reg_cnt, config->wr_buf);
        }
        if(ret < 0){
            dbg_infoln("第%d次测试:失败");
        }
        if(ret<0)
            err_cnt++;
    }
    dbg_infoln("测试%s:%d次,失败:%d次,耗时%.3fs", config->is_write?"写":"读", 
        config->test_cnt, err_cnt, ((float)(GET_TICK()-start_time))/1000);
    return 0;
}

static inline int _loader_ro_reg(RunConfig *config){
    return SpiReg_Read(&config->spi_reg_handle, 0x0000, sizeof(RoReg), config->wr_buf);
}

static int fun_wr(RunConfig *config){
    int ret;
    if(config->test_cnt){
            return _run_test(config);
        }

        if(config->is_write){
            ret = SpiReg_Write(&config->spi_reg_handle, config->reg_addr, config->reg_cnt, config->wr_buf);
            if(ret < 0){
                dbg_errfl("ERROR 写寄存器失败 ret = %d",ret);
                return -1;
            }
            
        }else{
            ret = SpiReg_Read(&config->spi_reg_handle, config->reg_addr, config->reg_cnt, config->wr_buf);
            if(ret < 0){
                dbg_errfl("ERROR 读寄存器失败 ret = %d",ret);
                return -1;
            }
            dbg_infoln("读到的数据:");
            dbg_infohex(config->wr_buf, config->reg_cnt);
        }
    return 0;
}

static int fun_show_mcu_ver(RunConfig *config){
    int ret;
    RoReg *ro_reg = (RoReg *)config->wr_buf;
    ret = _loader_ro_reg(config);
    if(ret < 0)
        dbg_errfl("_loader_ro_reg error! ret = %d",ret);
    dbg_inforaw("mcu ver: v%d.%d.%d\n", ro_reg->software_version.major,
                                      ro_reg->software_version.minor,
                                      ro_reg->software_version.patch );
    return 0;
}

static int fun_show_mcu_mode(RunConfig *config){
    int ret;
    RoReg *ro_reg = (RoReg *)config->wr_buf;
    ret = _loader_ro_reg(config);
    if(ret < 0)
        dbg_errfl("_loader_ro_reg error! ret = %d",ret);
    dbg_inforaw("mcu mode: %s\n",ro_reg->partition == 0? "bootloader":"app");

    return 0;
}

static int fun_show_mcu_info(RunConfig *config){
    int ret;
    RoReg *ro_reg = (RoReg *)config->wr_buf;
    ret = _loader_ro_reg(config);
    if(ret < 0)
        dbg_errfl("_loader_ro_reg error! ret = %d",ret);
    dbg_inforaw("mcu mode: %s\n",ro_reg->partition == 0? "bootloader":"app");
    dbg_inforaw("mcu ver: V%d.%d.%d\n", ro_reg->software_version.major,
                                      ro_reg->software_version.minor,
                                      ro_reg->software_version.patch );

    return 0;
}


int  run(RunConfig *config){

    if(config->mode == FUN_RW){
        return fun_wr(config);
    }else if(config->mode == FUN_SHOW_MCU_VER){
        return fun_show_mcu_ver(config);
    }else if(config->mode == FUN_SHOW_MCU_MODE){
        return fun_show_mcu_mode(config);
    }else if(config->mode == FUN_SHOW_MCU_INFO){
        return fun_show_mcu_info(config);
    }

    return 0;
}
