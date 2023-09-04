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




typedef enum _MBMBurnMode{
    BURNMODE_FORBID,             /* MCU置位 禁止烧写，MPU不得进行烧写 */
    BURNMODE_WAIT_BURN,          /* MCU置位 等待烧写模式，MPU可以写BURNMODE_START_BURN，使MCU进入烧写模式*/
    BURNMODE_START_ERASE,        /* MPU置位 擦除APP */
    BURNMODE_ERASE_OK,           /* MCU置位 MCU擦除成功后置位 */
    BURNMODE_START_BURN,         /* MPU置位 开始烧写模式，MPU可以传输数据 */
    BURNMODE_FINISH_TRANSFER,    /* MPU置位 完成发送，MPU传输完了数据 ,如果MCU完成了烧写可以设置为BURNMODE_FINISH_BURN*/
    BURNMODE_FINISH_BURN,        /* MCU置位 完成烧写*/
    BURNMODE_BURN_ERROR,         /* MCU置位 出错    */
    BURNMODE_EXIT_BURN,          /* MPU置位 退出烧写*/
    BURNMODE_APP_GOTO_BOOTLOADER = 0xFF /* 告诉APP,进入Bootloader 进入升级模式 */
}MBMBurnMode;

typedef enum _MBMErrorCode{
    MBMERROR_OK,
    MBMERROR_CRC_ERROR,
    MBMERROR_WRITE_ERROR,
    MBMERROR_TIMEOUT_ERROR,
    MBMERROR_ERASE_ERROR,
    MBMERROR_OTHER_ERROR,
}MBMErrorCode;

#pragma pack(1)
typedef struct _BurnStaReg{
    uint8_t  burn_mode;             /* 使用 MBMBurnMode  */
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