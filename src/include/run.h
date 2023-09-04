/**
 * @file run.h
 * @brief 
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-08-18
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

 #ifndef _RUN_H_
 #define _RUN_H_
 
 
 #ifdef __cplusplus
 #if __cplusplus
 extern "C"{
 #endif
 #endif /* __cplusplus */

#define WR_BUF_MAX 1024

enum RUN_FUN{
    FUN_RW,
    FUN_SHOW_MCU_INFO,
    FUN_UPDATE,
    FUN_FORCE_UPDATE,
};

typedef struct _RunConfig{
    uint8_t       wr_buf[WR_BUF_MAX];
    uint16_t      reg_addr;
    uint16_t      reg_cnt;
    uint32_t      test_cnt;
    int           is_write;
    int           is_show_mcu_info;
    int           is_read;
    char          *mcu_firmware;
    char          *mcu_force_firmware;
    enum RUN_FUN  mode;
}RunConfig;
 
extern int run(RunConfig *config);
 
 #ifdef __cplusplus
 #if __cplusplus
 }
 #endif
 #endif /* __cplusplus */
 
 
 #endif // _RUN_H_