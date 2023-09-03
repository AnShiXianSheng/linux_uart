/**
 * @file mpu-burn-mcu-task.h
 * @brief mpu 烧写 mcu实现，依赖于logic-reg.c的寄存器读写实现
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-09-02
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

#ifndef _MPU_BURN_MCU_TASK_H_
#define _MPU_BURN_MCU_TASK_H_

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */




#pragma pack(1)
typedef struct _BurnStaReg{
    uint8_t  is_goto_burn_mode;     /* 为1时进入烧写模式,2为写完毕,3为烧写完毕*/
    uint8_t  error_code;            /* 错误码 */
}BurnStaReg;
#pragma pack()

#define RWREG_BURN_START                0x1400
#define RWREG_CB_BURN_START             (0x1400 + sizeof(BurnStaReg))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _MPU-BURN-MCU-TASK_H_