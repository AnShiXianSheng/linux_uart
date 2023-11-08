/**
 * @file burn_flash.h
 * @brief 烧写flash的实现
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-09-04
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

#ifndef _BURN_FLASH_H_
#define _BURN_FLASH_H_

#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define BURN_BOOT_START         0x00000000
#define BURN_BOOT_END           0x00010000  /* 64K */
#define BURN_APP_A_START        0x00010000  
#define BURN_APP_A_END          0x00027800  /* 94K */
#define BURN_APP_B_START        0x00027800  
#define BURN_APP_B_END          0x0003F000  /* 94K */
#define BURN_BOOT_INFO_START    0x0003F000  
#define BURN_BOOT_INFO_END      0x0003F800  /* 2K */
#define BURN_CONST_INFO_START   0x0003F800  
#define BURN_CONST_INFO_END     0x00040000  /* 2K */



typedef struct _FlashPart{
    uint32_t addr_start;        /* 分区的开始地址 */
    uint32_t addr_end;          /* 分区的结束地址 */
    int      is_boot;           /* 分区是否允许启动 */
}FlashPart;

typedef enum _BurnFlashPart{
    BFP_BOOT,               /* boot程序所在分区 */
    BFP_APP_A,              /* app分区A */
    BFP_APP_B,              /* app分区B */
    BFP_BOOT_INFO,          /* 启动信息分区 */
    BFP_CONST_INFO,          /* 固定信息，理论上只能写一次 */
    BFP_MAX
}BurnFlashPart;

#define BURN_FLASH_PART_TAB  {                                                                                  \
    [BFP_BOOT] = {.is_boot = true, .addr_start = BURN_BOOT_START, .addr_end = BURN_BOOT_END},                   \
    [BFP_APP_A] = {.is_boot = true, .addr_start = BURN_APP_A_START, .addr_end = BURN_APP_A_END},                \
    [BFP_APP_B] = {.is_boot = true, .addr_start = BURN_APP_B_START, .addr_end = BURN_APP_B_END},                \
    [BFP_BOOT_INFO] = {.is_boot = false, .addr_start = BURN_BOOT_INFO_START, .addr_end = BURN_BOOT_INFO_END},    \
    [BFP_CONST_INFO] = {.is_boot = false, .addr_start = BURN_CONST_INFO_START, .addr_end = BURN_CONST_INFO_END}, \
}

extern int BurnFlash_Init(void);

extern uint32_t BurnFlash_PartAbsAddr(BurnFlashPart part);
extern int BurnFlash_ErasePart(BurnFlashPart part);
extern uint32_t BurnFlash_PartSize(BurnFlashPart part_num);
extern int BurnFlash_JumpExec(BurnFlashPart part);
extern int BurnFlash_ReadPart(BurnFlashPart part, uint32_t addr, uint8_t *buf, uint32_t size);
extern int BurnFlash_WritePart(BurnFlashPart part, uint32_t addr,const uint8_t *data, uint32_t size);


extern int BurnFlash_EEEWrite(uint32_t addr,const uint8_t *data, uint32_t size);
extern int BurnFlash_EEERead(uint32_t addr, uint8_t *buf, uint32_t buf_size);
extern uint32_t BurnFlash_EEEAbsAddr(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _BURN_FLASH_H_