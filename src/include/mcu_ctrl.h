/**
 * @file mcu_ctrl.h
 * @brief mcu控制接口
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */


#ifndef _MCU_CTRL_H_
#define _MCU_CTRL_H_


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define MCU_TTY_PARH    "/dev/ttyRPMSG31"

/* 摄像头状态表 */
#define MCU_CAMERA_CTRL_FENCE               0x00000000          /* 不确定态 */
#define MCU_CAMERA_CTRL_RESET               0x00000001          /* 复位状态 */
#define MCU_CAMERA_CTRL_RESET_TO_READY      0x00000002          /* 复位往就绪运行的状态 */
#define MCU_CAMERA_CTRL_READY               0x00000003          /* 就绪状态 */
#define MCU_CAMERA_CTRL_READY_TO_RESET      0x00000004          /* 就绪往复位运行的状态 */
#define MCU_CAMERA_CTRL_STUCK_TO_READY      0x00000005          /* 卡住往就绪运行的状态 */
#define MCU_CAMERA_CTRL_STUCK_TO_RESET      0x00000006          /* 卡住往复位运行的状态 */


typedef struct {
    union{
        uint32_t  state_map;
        struct {
            uint32_t camera:3;  /* 相机状态  具体查看 MCU_CAMERA_CTRL_XXXX*/
            uint32_t acc:1;     /* ACC状态   1为ACC输入，   0反之 */
            uint32_t can:1;     /* CAN状态   1为有CAN信号， 0反之 */
            uint32_t key:1;     /* KEY状态   1为有KEY信号， 0反之 */
            uint32_t door:1;    /* DOOR状态  1为有DOOR信号， 0反之 */
            uint32_t wdog:4;    /* wdog      0关看门狗，1-15为看门狗喂狗周期  */
        };
    };
    uint32_t power24v_adc_raw;  /* adc原始值，若采集失败时返回0xffffffff */
}McuState;

/* y=xk+b */
typedef struct{                 /* 校准参数结构体 */
	double  k;
	double  b;
}Calibration;





extern int      mcuctrl_MakeCalibration(Calibration *c, uint32_t adc_raw_24v, uint32_t adc_raw_36v);
extern double   mcuctrl_AdcRawToVoltage(const Calibration *c, uint32_t adc_raw);

extern int      mcuctrl_GetStateANDFood(McuState *mcu_state);
extern int      mcuctrl_SetWdogCycle(uint32_t cycle);
extern int      mcuctrl_SetWdogExceptionTimeout(uint32_t exception_timeout_s);
extern int      mcuctrl_CameraToReset(uint32_t best_time, int *is_busy);
extern int      mcuctrl_CameraToReady(uint32_t best_time, int *is_busy);
extern int      mcuctrl_Init(void);
extern void     mcuctrl_Exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _MCU_CTRL_H_

