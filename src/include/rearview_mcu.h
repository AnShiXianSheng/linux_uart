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

#include "stdint.h"
#include "mcu-reg/mcu-info.h"
#include "mcu-reg/mpu-burn-mcu.h"
#include "mcu-reg/can-event.h"
#include "mcu-reg/mpu-business.h"
#include "can-msg.h"
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


extern int RVMcu_ReceiveCanMsgBlock(PCanMsg *can_msg, uint32_t cnt,  uint32_t timeout);
extern int RVMcu_ReceiveCanMsg(PCanMsg *can_msg, uint32_t timeout);
extern int RVMcu_SendCanMsg(PCanMsg *can_msg, uint32_t timeout);
extern int RVMcu_WriteReg(uint16_t reg_addr, const uint8_t *reg_data, uint16_t reg_cnt, uint32_t timeout);
extern int RVMcu_ReadReg(uint16_t reg_addr,  uint8_t *reg_data, uint16_t reg_cnt, uint32_t timeout);
extern int RVMcu_BurnMcu(const char* mcu_firmware_path);
extern int RVMcu_ForceBurnMcu(const char* mcu_firmware_path);
extern int RVMcu_Init(void);
extern void RVMcu_Exit(void);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _REARVIEW_MCU_H_