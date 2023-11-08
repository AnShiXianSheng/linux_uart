/**
 * @file boot-info.h
 * @brief 启动分区的信息获取
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-09-11
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */


#include <stdint.h>

#ifndef _BOOT_INFO_H_
#define _BOOT_INFO_H_


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "burn_flash.h"

#define BOOT_MAGIC ('b'<< 0 | 'o'<< 8 |'o'<< 16 | 't' << 24)
#define PROGRAM_MAGIC ('p'<< 0 | 'i'<< 8 |'_'<< 16 | '_' << 24)

extern uint32_t __interrupts_start__;
extern uint32_t flash_config_start;

//#define PROGRAM_INFO_START_ADDR  (flash_config_start-__interrupts_start__)
#define PROGRAM_INFO_START_ADDR  (0x410U)
#define PROGRAM_INFO_END_ADDR    (0x490U)

#pragma pack(1)

typedef struct BootInfo{
    uint32_t boot_magic_a;
    BurnFlashPart boot_part;        /* 目前引导的app分区 */
    uint32_t boot_magic_b;
}BootInfo;


/**
 * @brief 结构体定义与MCU有所差异，这边指针直接使用uint32_t 来代替，因为64位上指针也是64位
 */
typedef struct _ProgramInfo{
    uint32_t            program_magic;         /* .program_info段开始 */
    uint32_t            program_len;           /* 整个程序的长度 */
    uint32_t            program_crc32;         /* 整个程序的长度 */
    struct {
        uint8_t         major;
        uint8_t         minor;
        uint8_t         patch;
        uint8_t         reserved;
    }software_version;
    uint32_t            compile_info;          /* 编译信息 */
    BurnFlashPart       part;                  /* 本程序应该在哪个分区 */
    uint32_t            comp_id;               /* 兼容id，只有兼容id一样的才能允许升级，否则就拒绝升级 */
    uint32_t            model;                 /* 软件型号 */
}ProgramInfo;

#pragma pack()

extern int BootInfo_SwitchBootPart(void);
extern int BootInfo_GetAppNextPart(BurnFlashPart *part);
extern int BootInfo_ExecCrc32Check(BurnFlashPart part);
extern int BootInfo_GetExecPartInfo(BurnFlashPart part, ProgramInfo *info);
extern int BootInfo_GetAppCurrentPart(BurnFlashPart *part);
extern void BootInfo_MakeDefaultBootInfoData(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _BOOT-INFO_H_