/**
 * @file rearview_mcu.h
 * @brief 
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-09-02
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

#ifndef _REARVIEW_MCU_H_
#define _REARVIEW_MCU_H_

#include <stdint.h>
#include "mcu-reg/mcu-info.h"
#include "mcu-reg/mpu-burn-mcu.h"
#include "mcu-reg/can-event.h"
#include "mcu-reg/mpu-business.h"
#include "mcu-reg/boot-info.h"
#include "can-msg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* MPU可控故障状态，设置与获取接口 */
extern int RVMcu_ReadMpuDtc(uint32_t* dtc_map);
extern int RVMcu_WriteMpuDtc(uint32_t dtc_map);

/* 看门狗开启与喂狗接口 */
extern int RVMcu_WdogFeed(void);
extern int RVMcu_WdogConfig(int is_on_wdog);
extern int RVMcu_WdogGetSta(int *is_on_wdog);

/* CAN发送与接受接口 */
extern int RVMcu_CleanRxFifo(uint32_t timeout);
extern int RVMcu_ReceiveCanMsgBlock(PCanMsg *can_msg, uint32_t cnt,  uint32_t timeout);
extern int RVMcu_ReceiveCanMsg(PCanMsg *can_msg, uint32_t timeout);
extern int RVMcu_SendCanMsgBlock(PCanMsg *can_msg, uint32_t cnt, uint32_t timeout);
extern int RVMcu_SendCanMsg(PCanMsg *can_msg, uint32_t timeout);

/* 寄存器读写接口 */
extern int RVMcu_WriteReg(uint16_t reg_addr, const uint8_t *reg_data, uint16_t reg_cnt, uint32_t timeout);
extern int RVMcu_ReadReg(uint16_t reg_addr,  uint8_t *reg_data, uint16_t reg_cnt, uint32_t timeout);

/* 烧写相关接口 */
extern int RVMcu_BurnMcu(const char* mcu_firmware_path);
extern int RVMcu_ForceBurnMcu(const char* mcu_firmware_path);

/* 杂项接口 */
extern int RVMcu_McuReset(void);
extern int RVMcu_SetRearviewType(int is_left_rearview);
extern int RVMcu_GetRearviewType(int *is_left_rearview);
extern int RVMcu_GetConfig80Byte(uint8_t *config_80byte );
extern int RVMcu_SetSupplyVoltage(uint32_t mv);   /* 毫伏，单位 */
extern int RVMcu_SetMcuDbgLevel(int level);
extern int RVMcu_CleanNvm(int nvm_index );

/* 为陕汽设计的接口 */
extern int RVMcu_ShanQiProductionDate(uint8_t production_date[4]);
extern int RVMcu_ShanQiCmsMsgSet(uint8_t msg_data[8]);

extern int RVMcu_Init(void);
extern void RVMcu_Exit(void);





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _REARVIEW_MCU_H_