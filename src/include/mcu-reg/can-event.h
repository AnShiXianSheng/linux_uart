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
    CET_IGNITION_POSITION,              /* 点火位置事件 匙位置寄存器：0:OFF 1:ACC 2:ON 3:START */ 
    CET_LEFT_DOOR_SWITCH,               /* 左门是否开关事件  0:关 1:开*/
    CET_LEFT_DOOR_LOCK,                 /* 左门是否锁事件    0:未锁 1:锁*/
    CET_RIGHT_DOOR_SWITCH,              /* 右门是否开关事件  0:关 1:开*/
    CET_RIGHT_DOOR_LOCK,                /* 右门是否锁事件    0:未锁 1:锁*/
    CET_ACC_PIN_H,                      /* 电路ACC引脚输入事件 0:无ACC 1:有ACC */
    CET_SCREEN_UP_DOWN_SWITCH_CNT,      /* SCREEN 开关屏 此按键唤醒的状态不受其他状态影响，强制亮屏120S 
                                         *   这里记录按键按下的次数 此按键在亮屏时按下应该息屏，在息屏时按下应该亮屏 
                                         */
    CET_MCU_STATE_MACHINE,              /* 状态机事件  指示状态机目前状态*/
    CET_CAN_BUS_VALID,                  /* CAN有效状态事件   0为CAN完全失效 */
    CET_OTHER_WAKE_UP_SOURCES_CNT,      /* 其他唤醒源唤醒计次 状态机若检测到计次增加则需要进行唤醒到 CIPE_MPU_RUN_NORMAL，然后根据当前状态自动过渡 */
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


#pragma pack(1)
typedef struct _RoRegCanEvent{
    uint8_t period_event_valid[64];          /* 周期事件有效寄存器 */
    uint8_t event[CAN_EVENT_USE_CNT];
}RoRegCanEvent;
#pragma pack()


extern void CanEvent_RegisterEventTimeout(const uint16_t timeout_tab[CAN_EVENT_USE_CNT]);
extern void CanEvent_SetEvent(CAN_EVENT_TYPE event_type, uint32_t event);
extern int CanEvent_GetEvent(CAN_EVENT_TYPE event_type, uint32_t* event);

extern void CanEvent_Proc(void);
extern int CanEvent_Init(void);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _CAN-EVENT_H_