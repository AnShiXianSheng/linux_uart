/**
 * @file can-event.h
 * @brief can事件处理，负责置位和管理相关寄存器
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-08-31
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */


#ifndef _CAN_EVENT_H_
#define _CAN_EVENT_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define ROREG_CAN_EVENT_START       0x0400

typedef enum _CAN_EVENT_TYPE{
    CET_IGNITION_POSITION,                  /* 点火位置事件 匙位置寄存器：0:OFF 1:ACC 2:ON 3:START */ 
    CET_LEFT_DOOR_SWITCH,                   /* 左门是否开关事件  0:关 1:开*/
    CET_LEFT_DOOR_LOCK,                     /* 左门是否锁事件    0:未锁 1:锁*/
    CET_RIGHT_DOOR_SWITCH,                  /* 右门是否开关事件  0:关 1:开*/
    CET_RIGHT_DOOR_LOCK,                    /* 右门是否锁事件    0:未锁 1:锁*/
    CET_ACC_PIN_H,                          /* 电路ACC引脚输入事件 0:无ACC 1:有ACC */
    CET_SCREEN_UP_DOWN_SWITCH_CNT,          /* SCREEN 开关屏 此按键唤醒的状态不受其他状态影响，强制亮屏120S 
                                             *   这里记录按键按下的次数 此按键在亮屏时按下应该息屏，在息屏时按下应该亮屏 
                                             */
    CET_MCU_STATE_MACHINE,                  /* 状态机事件  指示状态机目前状态*/
    CET_CAN_BUS_VALID,                      /* CAN有效状态事件   0为CAN完全失效 */
    CET_OTHER_WAKE_UP_SOURCES_CNT,          /* 其他唤醒源唤醒计次 状态机若检测到计次增加则需要进行唤醒到 CIPE_MPU_RUN_NORMAL，然后根据当前状态自动过渡 */

    CET_CAN_BUS_DCM_VALID,                  /* CAN 门窗控制器有效标识 0为门窗控制器CAN无效 */
    CET_CAN_BUS_GW_VALID,                   /* CAN GW控制器有效标识 0为GW控制器CAN无效 */

    CET_VEHICLE_SPEED_U16_L,                /* 车速 uint16_t 类型L 0-500 km/h */
    CET_VEHICLE_SPEED_U16_H,                /* 车速 uint16_t 类型H  */

    CET_TURN_SPEED_U16_L,                   /* 发动机转速 uint16_t 类型L 0-10000 rpm */
    CET_TURN_SPEED_U16_H,                   /* 发动机转速 uint16_t 类型H  */

    CET_TIME_SEC,                           /* 秒 0-59 s */
    CET_TIME_MIN,                           /* 分 0-59 m */
    CET_TIME_HOUR,                          /* 时 0-23 h */
    CET_TIME_DAY,                           /* 日 1-31 D */
    CET_TIME_MONTH,                         /* 月 1-12 D */
    CET_TIME_YEAR,                          /* 年 0-255 offset: 1985-2240年   */

    CET_DC_VOL_U16_L,                       /* 电源电压低位 0-65535   0.1v/bit 0.0-6553.5 v*/
    CET_DC_VOL_U16_H,                       /* 电源电压高位 */

    CET_ROAD_HAUL_U32_BYTE0,                /* 行驶里程数 使用 u32的值乘5米，就是最终的里程数*/
    CET_ROAD_HAUL_U32_BYTE1,                /* 行驶里程数 */
    CET_ROAD_HAUL_U32_BYTE2,                /* 行驶里程数 */
    CET_ROAD_HAUL_U32_BYTE3,                /* 行驶里程数 */

    CET_REARVIEW_SELECT,                    /* 后视镜选择信号 枚举值 CET_REARVIEW_SELECT_EVENT*/

    CET_REARVIEW_VERTICAL_ADJUSTMENT,       /* 后视镜上下调节信号 */
    CET_REARVIEW_HORIZONTAL_ADJUSTMENT,     /* 后视镜左右调节信号 */

    CET_REVERSE_LIGHT,                      /* 倒车灯 CET_REVERSE_LIGHT_EVENT*/
    CET_LEFT_TURN_LIGHT,                    /* 左转向灯  CET_RIGHT_TURN_LIGHT_EVENT*/
    CET_RIGHT_TURN_LIGHT,                   /* 右转向灯  CET_RIGHT_TURN_LIGHT_EVENT*/
    CET_POSITION_LIGHT,                     /* 位置灯 */
    CET_R1_LIGHT,                           /* 预留灯 */
    CET_R2_LIGHT,                           /* 预留灯 */
    CET_R3_LIGHT,                           /* 预留灯 */
    CET_R4_LIGHT,                           /* 预留灯 */

    CET_STEERING_ANGLE_I16_L,               /* 方向盘角度 -32767 --> 32767 */
    CET_STEERING_ANGLE_I16_H,               /* 方向盘角度 */

    CET_ENGINE_RUN_TIME_U32_BYTE0,          /* 发动机 运行时间单位秒 */
    CET_ENGINE_RUN_TIME_U32_BYTE1,          /* 发动机 运行时间单位秒 */
    CET_ENGINE_RUN_TIME_U32_BYTE2,          /* 发动机 运行时间单位秒 */
    CET_ENGINE_RUN_TIME_U32_BYTE3,          /* 发动机 运行时间单位秒 */

    CET_GEARS,                              /* 汽车档位 CET_GEARS_EVENT*/

    CET_COUNTDOWN_U16_L,                    /* 倒计时事件 */
    CET_COUNTDOWN_U16_H,

    CET_MAX,
}CAN_EVENT_TYPE;

#define CAN_EVENT_MAX_CNT                   512         /* CAN事件的最大数量 */
#define CAN_EVENT_USE_CNT                   CET_MAX     /* 目前使用的can事件数量 */

typedef enum _CET_IGNITION_POSITION_EVENT{
    CIPE_OFF,
    CIPE_ACC,
    CIPE_ON,
    CIPE_START,
}CET_IGNITION_POSITION_EVENT;

typedef enum _CET_STATE_MACHINE_EVENT{
    CIPE_JUST_BOOT,                             /* MCU刚上电等待CAN报文 */
    CIPE_DORMANCY,                              /* (息屏)休眠模式 */
    CIPE_MPU_RUN_NORMAL,                        /* MPU正常工作状态 */
    CIPE_MPU_RUN_WAIT_SCREEN_OFF,               /* 等待关屏 通常120倒计时 */
    CIPE_MPU_RUN_WAIT_DORMANCY,                 /* 等待休眠 通常300倒计时*/
    CIPE_MPU_RUN_FORCE_WAIT_SCREEN_OFF,         /* 强制等待关屏，只通过倒计时进入 CIPE_MPU_RUN_WAIT_POEWR_OFF */
    CIPE_MPU_RUN_FORCE_SCREEN_OFF,              /* 强制关屏 目前MCU不进行任何处理 */
    CIPE_MPU_RUN_CAN_ERROR,                     /* CAN失效状态，强制亮屏，只有在CIPE_MPU_RUN_NORMAL状态下，且CAN失效才进入该状态 */
}CET_STATE_MACHINE_EVENT;

typedef enum _CET_DOOR_LOCK_EVENT{
    CDLE_DOOR_UNLOCK,
    CDLE_DOOR_LOCK
}CET_DOOR_LOCK_EVENT;

typedef enum _CET_DOOR_SWITCH_EVENT{
    CDSE_DOOR_OFF,
    CDSE_DOOR_ON,
}CET_DOOR_SWITCH_EVENT;

typedef enum _CET_CAN_BUS_VALID_EVENT{
    CCBVE_INVALID    = 0x00000000,
    CCBVE_CAN_ACTIVE = 0x00000001,
}CET_CAN_BUS_VALID_EVENT;

typedef enum _CET_REARVIEW_SELECT{
    CRS_NONE,                       /* 没有选择任何后视镜 */
    CRS_LEFT,                       /* 选择了左边的后视镜 */
    CRS_RIGHT,                      /* 选择了右边的后视镜 */
    CRS_ALL,                        /* 选择了全部的后视镜 */
}CET_REARVIEW_SELECT_EVENT;

typedef enum _CET_REARVIEW_VERTICAL_ADJUSTMENT{
    CRVA_NONE,                      /* 没有任何调节信号 */
    CRVA_UP_ADJUSTMENT,             /* 后视镜向上调节 */
    CRVA_DOWN_ADJUSTMENT            /* 后视镜向下调节 */
}CET_REARVIEW_VERTICAL_ADJUSTMENT_EVENT;

typedef enum _CET_REARVIEW_HORIZONTAL_ADJUSTMENT{
    CRHA_NONE,                      /* 没有任何调节信号 */
    CRHA_LEFT_ADJUSTMENT,           /* 后视镜向左调节 */
    CRHA_RIGHT_ADJUSTMENT           /* 后视镜向右调节 */
}CET_REARVIEW_HORIZONTAL_ADJUSTMENT_EVENT;

typedef enum _CET_REVERSE_LIGHT{
    CRLE_OFF,                       /* 倒车灯灭 */
    CRLE_ON,                        /* 倒车灯亮 */
    CRLE_INVALID,                   /* 倒车灯无效 */
}CET_REVERSE_LIGHT_EVENT;


typedef enum _CET_LEFT_TURN_LIGHT{
    CLTLE_OFF,                      /* 左转向灯灭 */
    CLTLE_NORMAL_BLINK,             /* 正常闪烁 */
    CLTLE_FAST_BLINK,               /* 快速闪烁 */
    CLTLE_BRIGHT,                   /* 左转向灯亮 */
    CLTLE_INVALID,                  /* 左转向灯无效 */
}CET_LEFT_TURN_LIGHT_EVENT;

typedef enum _CET_RIGHT_TURN_LIGHT{
    CRTLE_OFF,                      /* 右转向灯灭 */
    CRTLE_NORMAL_BLINK,             /* 正常闪烁 */
    CRTLE_FAST_BLINK,               /* 快速闪烁 */
    CRTLE_BRIGHT,                   /* 右转向灯亮 */
    CRTLE_INVALID,                  /* 右转向灯无效 */
}CET_RIGHT_TURN_LIGHT_EVENT;


typedef enum _CET_GEARS_EVENT{
    CGE_NEUTRAL,                    /* 空档 */
    CGE_REVERSE,                    /* 倒车档 */
    CGE_OTHER,                      /* 其他档 */
}CET_GEARS_EVENT;



#pragma pack(1)
typedef struct _RoRegCanEvent{
    uint8_t period_event_valid[64];          /* 周期事件有效寄存器 */
    uint8_t event[CAN_EVENT_USE_CNT];
}RoRegCanEvent;
#pragma pack()


extern void CanEvent_RegisterEventTimeout(const uint16_t timeout_tab[CAN_EVENT_USE_CNT]);
extern void CanEvent_SetEvent(CAN_EVENT_TYPE event_type, uint8_t event);
extern int CanEvent_GetEvent(CAN_EVENT_TYPE event_type, uint8_t* event);
extern void CanEvent_SetGroupEvent(CAN_EVENT_TYPE event_type, uint32_t group_len, uint8_t *event);
extern int CanEvent_GetGroupEvent(CAN_EVENT_TYPE event_type, uint32_t group_len, uint8_t *event);
extern void CanEvent_Proc(void);
extern int CanEvent_Init(void);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _CAN-EVENT_H_