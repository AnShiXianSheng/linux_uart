/**
 * @file mcu_info.h
 * @brief mcu info 寄存器的结构
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-09-02
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

#ifndef _MCU_INFO_H_
#define _MCU_INFO_H_


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "stdint.h"

#define ROREG_INFO_START                0x0000
#define RWREG_INFO_START                0x1000
#define MCU_INFO_SN_SIZE                32
#define MCU_INFO_SOFTWARE_MODEL_SIZE    32

#pragma pack(1)
typedef struct _McuInfo{
    struct {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
        uint8_t reserved;
    }software_version;
    char        sn[MCU_INFO_SN_SIZE];
    uint32_t    partition;  /* 当前分区 0：bootloader 1:app */
    char        software_model[MCU_INFO_SOFTWARE_MODEL_SIZE];
}McuInfo;

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _MCU_INFO_H_