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

#include "bits.h"
#include "mcu-reg/can-event.h"
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
    uint32_t i;
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
    dbg_inforaw("mcu mode: %s\n", mcu_info->partition == 0? "bootloader":
                                  mcu_info->partition == 1? "app_A": "app_B"       );
    dbg_inforaw("mcu sn: %s\n",mcu_info->sn);
    dbg_inforaw("mcu software model: %s\n",mcu_info->software_model);
    dbg_inforaw("mcu ver: V%d.%d.%d\n", mcu_info->software_version.major,
                                      mcu_info->software_version.minor,
                                      mcu_info->software_version.patch );

    return 0;
}

static int fun_show_mcu_can_event(RunConfig *config){
    int ret;
    (void) config;
    RoRegCanEvent can_event;
    ret = RVMcu_ReadReg(ROREG_CAN_EVENT_START, (uint8_t*)&can_event, sizeof(can_event), 200);
    if(ret < 0){
        dbg_errfl("SpiReg_Read error! ret = %d",ret);
        return -1;
    }
    if(Get_Bit(can_event.period_event_valid, CET_IGNITION_POSITION))
        dbg_infoln("点火位置： %s", can_event.event[CET_IGNITION_POSITION] == CIPE_OFF ?     "CIPE_OFF":
                                   can_event.event[CET_IGNITION_POSITION] == CIPE_ACC ?     "CIPE_ACC":
                                   can_event.event[CET_IGNITION_POSITION] == CIPE_ON ?      "CIPE_ON":
                                   can_event.event[CET_IGNITION_POSITION] == CIPE_START ?   "CIPE_START": "ERR" );

    if(Get_Bit(can_event.period_event_valid, CET_LEFT_DOOR_SWITCH))
        dbg_infoln("左门状态： %s", can_event.event[CET_LEFT_DOOR_SWITCH] == CDSE_DOOR_OFF ?     "CDSE_DOOR_OFF": "CDSE_DOOR_ON");
    
    if(Get_Bit(can_event.period_event_valid, CET_LEFT_DOOR_LOCK))
        dbg_infoln("左门锁状态： %s", can_event.event[CET_LEFT_DOOR_LOCK] == CDLE_DOOR_UNLOCK ?     "CDLE_DOOR_UNLOCK": "CDLE_DOOR_LOCK");
    
    if(Get_Bit(can_event.period_event_valid, CET_RIGHT_DOOR_SWITCH))
        dbg_infoln("右门状态： %s", can_event.event[CET_RIGHT_DOOR_SWITCH] == CDSE_DOOR_OFF ?     "CDSE_DOOR_OFF": "CDSE_DOOR_ON");
    
    if(Get_Bit(can_event.period_event_valid, CET_RIGHT_DOOR_LOCK))
        dbg_infoln("右门锁状态： %s", can_event.event[CET_RIGHT_DOOR_LOCK] == CDLE_DOOR_UNLOCK ?     "CDLE_DOOR_UNLOCK": "CDLE_DOOR_LOCK");

    
    if(Get_Bit(can_event.period_event_valid, CET_ACC_PIN_H))
        dbg_infoln("ACC %s", can_event.event[CET_ACC_PIN_H] ?     "H": "L");
    
    
    if(Get_Bit(can_event.period_event_valid, CET_SCREEN_UP_DOWN_SWITCH_CNT))
        dbg_infoln("开关屏按下次数(只能记录到0xff自动溢出):%d", can_event.event[CET_SCREEN_UP_DOWN_SWITCH_CNT]);

    if(Get_Bit(can_event.period_event_valid, CET_MCU_STATE_MACHINE))
        dbg_infoln("MCU状态机状态: %s",  can_event.event[CET_MCU_STATE_MACHINE] == CIPE_JUST_BOOT ? "刚上电状态": 
                                        can_event.event[CET_MCU_STATE_MACHINE] == CIPE_DORMANCY ? "休眠状态":    
                                        can_event.event[CET_MCU_STATE_MACHINE] == CIPE_MPU_RUN_NORMAL ? "正常工作状态":    
                                        can_event.event[CET_MCU_STATE_MACHINE] == CIPE_MPU_RUN_WAIT_SCREEN_OFF ? "等待关屏状态 120S倒计时":    
                                        can_event.event[CET_MCU_STATE_MACHINE] == CIPE_MPU_RUN_WAIT_DORMANCY ? "等待休眠状态 300S倒计时":   
                                        can_event.event[CET_MCU_STATE_MACHINE] == CIPE_MPU_RUN_FORCE_WAIT_SCREEN_OFF ? "强制等待关屏状态 120S倒计时":
                                        "错误状态");
    
    if(Get_Bit(can_event.period_event_valid, CET_CAN_BUS_VALID))
        dbg_infoln("CAN有效: %s", can_event.event[CET_CAN_BUS_VALID] ? "有效":"无效");

    if(Get_Bit(can_event.period_event_valid, CET_OTHER_WAKE_UP_SOURCES_CNT))
        dbg_infoln("其他唤醒源唤醒次数: %d", can_event.event[CET_OTHER_WAKE_UP_SOURCES_CNT]);
    
    if(Get_Bit(can_event.period_event_valid, CET_CAN_BUS_DCM_VALID))
        dbg_infoln("门窗控制器有效: %s", can_event.event[CET_CAN_BUS_DCM_VALID] ? "有效":"无效");

    
    if(Get_Bit(can_event.period_event_valid, CET_CAN_BUS_GW_VALID))
        dbg_infoln("网关有效: %s", can_event.event[CET_CAN_BUS_GW_VALID] ? "有效":"无效");

    if(Get_Bit(can_event.period_event_valid, CET_VEHICLE_SPEED_U16_L) &&
       Get_Bit(can_event.period_event_valid, CET_VEHICLE_SPEED_U16_H))
        dbg_infoln("车速: %d km/h", can_event.event[CET_VEHICLE_SPEED_U16_L] | (can_event.event[CET_VEHICLE_SPEED_U16_H] << 8));

    if(Get_Bit(can_event.period_event_valid, CET_TURN_SPEED_U16_L) &&
       Get_Bit(can_event.period_event_valid, CET_TURN_SPEED_U16_H))
        dbg_infoln("发动机转速: %d rpm", can_event.event[CET_TURN_SPEED_U16_L] | (can_event.event[CET_TURN_SPEED_U16_H] << 8));

    if( Get_Bit(can_event.period_event_valid, CET_TIME_SEC) || 
        Get_Bit(can_event.period_event_valid, CET_TIME_MIN) || 
        Get_Bit(can_event.period_event_valid, CET_TIME_HOUR) || 
        Get_Bit(can_event.period_event_valid, CET_TIME_DAY) || 
        Get_Bit(can_event.period_event_valid, CET_TIME_MONTH) || 
        Get_Bit(can_event.period_event_valid, CET_TIME_YEAR)    ){
        
        dbg_infoln("时间: %02d/%02d/%02d %02d:%02d:%02d", can_event.event[CET_TIME_YEAR] + 1985,
                        can_event.event[CET_TIME_MONTH], can_event.event[CET_TIME_DAY], can_event.event[CET_TIME_HOUR],
                        can_event.event[CET_TIME_MIN], can_event.event[CET_TIME_SEC] );
    }

    if( Get_Bit(can_event.period_event_valid, CET_DC_VOL_U16_L) || 
        Get_Bit(can_event.period_event_valid, CET_DC_VOL_U16_H) ){
        dbg_infoln("电源电压: %.1fv", (can_event.event[CET_DC_VOL_U16_L] | (can_event.event[CET_DC_VOL_U16_H] << 8))*0.1 );
    }

    if( Get_Bit(can_event.period_event_valid, CET_ROAD_HAUL_U32_BYTE0) || 
        Get_Bit(can_event.period_event_valid, CET_ROAD_HAUL_U32_BYTE1) || 
        Get_Bit(can_event.period_event_valid, CET_ROAD_HAUL_U32_BYTE2) || 
        Get_Bit(can_event.period_event_valid, CET_ROAD_HAUL_U32_BYTE3) ){
        uint32_t rh_u32;
        uint64_t rh_u64;
        rh_u32 =    can_event.event[CET_ROAD_HAUL_U32_BYTE0]             | 
                    (can_event.event[CET_ROAD_HAUL_U32_BYTE1]  <<  8)    | 
                    (can_event.event[CET_ROAD_HAUL_U32_BYTE2]  << 16)    | 
                    (can_event.event[CET_ROAD_HAUL_U32_BYTE3]  << 24) ;
        rh_u64 = rh_u32*5;
        dbg_infoln("总里程: %llu", rh_u64 );
    }

    return 0;
}



int  run(RunConfig *config){
    int ret;
    if(config->mode == FUN_RW){
        return fun_wr(config);
    }else if(config->mode == FUN_SHOW_MCU_INFO){
        return fun_show_mcu_info(config);
    }else if(config->mode == FUN_UPDATE){
        ret = RVMcu_BurnMcu(config->mcu_firmware);
        if(ret < 0) dbg_errfl("烧写失败!");
        return ret;
    }else if(config->mode == FUN_FORCE_UPDATE){ 
        return RVMcu_ForceBurnMcu(config->mcu_force_firmware);
    }else if(config->mode == FUN_READ_CAN_EVENT){
        return fun_show_mcu_can_event(config);
    }

    return 0;
}
