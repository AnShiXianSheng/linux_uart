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
#include "rearview_mcu.h"

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
            ret = RVMcu_WriteReg(config->reg_addr, config->wr_buf, config->reg_cnt, 200);
        }else{
            ret = RVMcu_ReadReg(config->reg_addr, config->wr_buf, config->reg_cnt, 200);
        }
        if(ret < 0){
            dbg_infoln("第%d次测试:失败",i);
        }
        if(ret<0)
            err_cnt++;
    }
    dbg_infoln("测试%s:%d次,失败:%d次,耗时%.3fs", config->is_write?"写":"读", 
        config->test_cnt, err_cnt, ((float)(GET_TICK()-start_time))/1000);
    return 0;
}

static int fun_wr(RunConfig *config){
    int ret;
    if(config->test_cnt){
            return _run_test(config);
        }

        if(config->is_write){
            ret = RVMcu_WriteReg(config->reg_addr, config->wr_buf, config->reg_cnt, 200);
            if(ret < 0){
                dbg_errfl("ERROR 写寄存器失败 ret = %d",ret);
                return -1;
            }
            
        }else{
            ret = RVMcu_ReadReg(config->reg_addr, config->wr_buf, config->reg_cnt, 200);
            if(ret < 0){
                dbg_errfl("ERROR 读寄存器失败 ret = %d",ret);
                return -1;
            }
            dbg_infoln("读到的数据:");
            dbg_infohex(config->wr_buf, config->reg_cnt);
        }
    return 0;
}

static int fun_show_mcu_info(RunConfig *config){
    int ret;
    McuInfo *mcu_info = (McuInfo *)config->wr_buf;
    ret = RVMcu_ReadReg(ROREG_INFO_START, (uint8_t*)mcu_info, sizeof(McuInfo), 200);
    if(ret < 0){
        dbg_errfl("SpiReg_Read error! ret = %d",ret);
        return -1;
    }
    dbg_inforaw("mcu mode: %s\n",mcu_info->partition == 0? "bootloader":"app");
    dbg_inforaw("mcu sn: %s\n",mcu_info->sn);
    dbg_inforaw("mcu software model: %s\n",mcu_info->software_model);
    dbg_inforaw("mcu ver: V%d.%d.%d\n", mcu_info->software_version.major,
                                      mcu_info->software_version.minor,
                                      mcu_info->software_version.patch );

    return 0;
}


int  run(RunConfig *config){

    if(config->mode == FUN_RW){
        return fun_wr(config);
    }else if(config->mode == FUN_SHOW_MCU_INFO){
        return fun_show_mcu_info(config);
    }else if(config->mode == FUN_UPDATE){
        return RVMcu_BurnMcu(config->mcu_firmware);
    }else if(config->mode == FUN_FORCE_UPDATE){ 
        return RVMcu_ForceBurnMcu(config->mcu_force_firmware);
    }

    return 0;
}
