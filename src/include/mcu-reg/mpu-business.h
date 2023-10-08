/**
 * @file mpu-business.h
 * @brief 
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-09-25
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */
#ifndef _MPU_BUSINESS_H_
#define _MPU_BUSINESS_H_


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define RWREG_MPU_BUSINESS_START                        0x1000
#define RWREG_CB_MPU_BUSINESS_SEND_CAN_START            ((RWREG_MPU_BUSINESS_START) + 0x00)
#define RWREG_CB_MPU_BUSINESS_RECEIVE_CAN_START         ((RWREG_CB_MPU_BUSINESS_SEND_CAN_START) + 0x08)
#define RWREG_MPU_BUSINESS_REG_START                    ((RWREG_CB_MPU_BUSINESS_RECEIVE_CAN_START) + 0x08)


#define RWREG_DTC_CNT_SIZE                              12

#define MPU_DTC_MAP_MASK                                0x0000078FU


#pragma pack(1)
typedef struct _MpuBusinessReg{
    uint8_t is_allow_send;                              /* 建议MPU只读，是否允许发送can报文，MPU见到此标志被置位时，应该停止发送CAN报文，即使发送也会被MCU清除 */
    uint8_t mpu_online_cnt;                             /* mpu在线,当mpu在线时应该对该值进行++ */
    uint8_t offline_timeout_reset;                      /* 离线超时复位 0使能 1使能 */
    uint8_t reserve[1];                                 /* 保留 */
    uint32_t dtc_map;                                   /* 故障位图 bit0对应1号故障,依此类推到12号故障,当故障存在时应该置相应位为1，故障消失时置相应位为0*/
}MpuBusinessReg;
#pragma pack()

extern void     MpuBusiness_Proc(void);
extern int      MpuBusiness_Init(void);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _MPU-BUSINESS_H_