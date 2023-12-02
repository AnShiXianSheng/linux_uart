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
 
 
 #include <stdint.h>
#ifdef __cplusplus
 #if __cplusplus
 extern "C"{
 #endif
 #endif /* __cplusplus */

#define WR_BUF_MAX 1024

enum RUN_FUN{
    FUN_MPU_ONLINE, 
    FUN_RW,
    FUN_SHOW_MCU_INFO,
    FUN_UPDATE,
    FUN_FORCE_UPDATE,
    FUN_READ_CAN_EVENT,
    FUN_LOOK_MPU_DTC,
    FUN_SET_OR_CLEAN_MPU_DTC,
    FUN_SEND_CAN_MSG,
    FUN_LOOP_RECEIVE_CAN_MSG,
    FUN_SETTING_WDOG,
    FUN_RESET_MCU,
    FUN_SET_REARVIEW_TYPE,
    FUN_SET_MCU_DEBUG_LEVEL,
    FUN_EXIT_BOOTLOADER,
    FUN_GOTO_BOOTLOADER,
    FUN_CLEAN_NVM,
    FUN_CAN_ECHO_TEST,
    FUN_WRITE_SHANQI_PRODUCTION_DATE,
};

typedef struct _RunConfig{
    uint8_t       wr_buf[WR_BUF_MAX];
    uint32_t      reg_addr;            /* 当发送can报文时为CAN ID */
    uint16_t      reg_cnt;
    uint32_t      test_cnt;
    uint32_t      set_dtc;
    uint32_t      rearview_type;
    uint32_t      clean_dtc;
    uint32_t      clean_nvm;
    int           mcu_debug_level;
    int           is_write;
    int           is_show_mcu_info;
    int           is_look_dtc;
    int           is_loop_reveive;
    int           is_can_print_asc;
    int           is_read;
    int           is_send_can;
    int           is_read_can_event;
    int           is_opne_wdog;
    int           is_close_wdog;
    int           is_reset_mcu;
    int           is_exit_bootloader;
    int           is_goto_bootloader;
    int           is_can_echo_test;
    int           is_write_shanqi_production_date;
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