/**
 * @file main.c
 * @brief 测试主逻辑
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>


#include "memctrl.h"
#include "debug.h"
#include "argparse.h"
#include "spi_reg.h"
#include "rearview_mcu.h"

#include "run.h"


static const char* const usages[] = {
    "mcu_reg_wr [-t] <-w|r|v|e> [REG ADDR] [WRITE DATA|READ LEN]",
    NULL,
};

static int hex_string_to_byte_array(const char *hexString, unsigned char *byteArray, size_t byteArraySize) {
    size_t len = strlen(hexString);
    char *endptr;

    if (len % 2 != 0 || len / 2 > byteArraySize)
        return -1;

    for (size_t i = 0; i < len; i += 2) {
        char byteStr[3] = {hexString[i], hexString[i + 1], '\0'};
        byteArray[i / 2] = (unsigned char)strtol(byteStr, &endptr, 16);
        if(byteStr == endptr)
            return -1;
    }
    return len / 2;
}

static int other_argparse_parse(RunConfig* config, int argc, const char* argv[]){
    char *endptr;
    if((config->is_write && config->is_read))
        return -1;


    if(config->is_write || config->is_read || config->is_send_can ){
        if(argc != 2) return -1;
        config->reg_addr = strtoul(argv[0], &endptr, 0);
        if(argv[0] == endptr)
            return -1;
        
        if(config->test_cnt || config->is_read){
            config->reg_cnt = strtol(argv[1], &endptr, 0);
            if(argv[1] == endptr || config->reg_cnt > (WR_BUF_MAX-2) || config->reg_cnt == 0)
            {
                dbg_inforaw("ERROR: 请输入合理的大小 config->reg_cnt = %d\n",config->reg_cnt);
                return -1;
            }
        }else{
            int reg_cnt =hex_string_to_byte_array(argv[1], config->wr_buf, WR_BUF_MAX);
            config->reg_cnt = (uint32_t)reg_cnt;
            if(reg_cnt < 0){
                dbg_inforaw("ERROR: 请勿超过%d个字节,请勿出现奇数个16进制!!\n",WR_BUF_MAX-2);
                return -1;
            }
        }
    }

    if(config->is_write_shanqi_production_date){
        int reg_cnt;
        if(argc != 1) return -1;
        
        reg_cnt =hex_string_to_byte_array(argv[0], config->wr_buf, 4);
        if(reg_cnt != 4){
            dbg_inforaw("ERROR: 请输入生产日期比如20231202\n");
            return -1;
        }
    }

    if(config->is_show_mcu_info){
        config->mode = FUN_SHOW_MCU_INFO;
        return 0;
    }
    if(config->is_write || config->is_read){
        config->mode = FUN_RW;
    }
    if(config->mcu_firmware){
        config->mode = FUN_UPDATE;
    }
    if(config->mcu_force_firmware){
        config->mode = FUN_FORCE_UPDATE;
    }
    if(config->is_read_can_event){
        config->mode = FUN_READ_CAN_EVENT;
    }
    if(config->is_look_dtc){
        config->mode = FUN_LOOK_MPU_DTC;
    }
    if(config->set_dtc || config->clean_dtc){
        config->mode = FUN_SET_OR_CLEAN_MPU_DTC;
    }
    if(config->is_send_can){
        config->mode = FUN_SEND_CAN_MSG;
    }
    if(config->is_loop_reveive){
        config->mode = FUN_LOOP_RECEIVE_CAN_MSG;
    }
    if(config->is_opne_wdog || config->is_close_wdog){
        config->mode = FUN_SETTING_WDOG;
    }
    if(config->is_reset_mcu ){
        config->mode = FUN_RESET_MCU;
    }
    if(config->rearview_type != 0xFFFFFFFF ){
        config->mode = FUN_SET_REARVIEW_TYPE;
    }
    if(config->is_goto_bootloader){
        config->mode = FUN_GOTO_BOOTLOADER;
    }
    if(config->is_exit_bootloader){
        config->mode = FUN_EXIT_BOOTLOADER;
    }
    if(config->mcu_debug_level != -1){
        config->mode = FUN_SET_MCU_DEBUG_LEVEL;
    }
    if(config->clean_nvm != 0){
        config->mode = FUN_CLEAN_NVM;
    }
    if(config->is_can_echo_test){
        config->mode = FUN_CAN_ECHO_TEST;
    }
    if(config->is_write_shanqi_production_date){
        config->mode = FUN_WRITE_SHANQI_PRODUCTION_DATE;
    }
    return 0;
}

int main(int argc, const char* argv[]){
    int ret;
    struct argparse argparse;
    RunConfig  run_config = {
        .is_read = 0, .is_write = 0,
        .test_cnt = 0,.wr_buf = {0},
        .is_read_can_event = 0,
        .reg_addr = 0x0000, .reg_cnt = 0, 
        .mcu_firmware = NULL,
        .mcu_force_firmware = NULL,
        .set_dtc = 0,
        .clean_dtc = 0,
        .clean_nvm = 0,
        .is_look_dtc = 0,
        .mode = FUN_MPU_ONLINE,
        .is_send_can = 0,
        .is_loop_reveive = 0,
        .is_can_print_asc = 0,
        .is_opne_wdog = 0,
        .is_close_wdog = 0,
        .is_reset_mcu = 0,
        .is_exit_bootloader = 0,
        .is_goto_bootloader = 0,
        .is_can_echo_test = 0,
        .is_write_shanqi_production_date = 0,
        .rearview_type = 0xFFFFFFFF,
        .mcu_debug_level = -1,
    };
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("基本命令"),
        OPT_BOOLEAN('w', "write", &run_config.is_write, "写寄存器", NULL, 0, 0),
        OPT_BOOLEAN('r', "read", &run_config.is_read, "读寄存器", NULL, 0, 0),
        OPT_BOOLEAN('m', "send-can-msg", &run_config.is_send_can, "发送CAN报文", NULL, 0, 0),
        OPT_BOOLEAN('d', "reveive-can-msg", &run_config.is_loop_reveive, "接收CAN报文", NULL, 0, 0),
        OPT_BOOLEAN('a', "can-print-asc", &run_config.is_can_print_asc, "在使用-d时加此选项可以选择打印asc格式的报文", NULL, 0, 0),
        OPT_BOOLEAN('c', "read-can-event", &run_config.is_read_can_event, "读所有CAN事件", NULL, 0, 0),
        OPT_BOOLEAN('i', "show-mcu-info", &run_config.is_show_mcu_info, "显示mcu所有信息", NULL, 0, 0),
        OPT_BOOLEAN('l', "look-mpu-dtc", &run_config.is_look_dtc, "显示MPU故障", NULL, 0, 0),
        OPT_BOOLEAN('o', "open-wdog", &run_config.is_opne_wdog, "打开看门狗", NULL, 0, 0),
        OPT_BOOLEAN('z', "close-wdog", &run_config.is_close_wdog, "关闭看门狗", NULL, 0, 0),
        OPT_BOOLEAN('b', "mcu-reset", &run_config.is_reset_mcu, "MCU复位", NULL, 0, 0),
        OPT_BOOLEAN('y', "exit-bootloader", &run_config.is_exit_bootloader, "如果在BOOTLOADER中此命令将可以退出BOOTLOADER", NULL, 0, 0),
        OPT_BOOLEAN('f', "goto-bootloader", &run_config.is_goto_bootloader, "如果在APP中此命令将可以进入BOOTLOADER", NULL, 0, 0),
        OPT_BOOLEAN('T', "can-echo-test", &run_config.is_can_echo_test, "CAN报文ECHO测试", NULL, 0, 0),
        OPT_BOOLEAN(' ', "write-shanqi-production-date", &run_config.is_write_shanqi_production_date, "写陕汽的生产日期", NULL, 0, 0),
        OPT_STRING('u', "update", &run_config.mcu_firmware, "升级固件", NULL, 0, 0),
        OPT_STRING('U', "Update", &run_config.mcu_force_firmware, "强行升级固件", NULL, 0, 0),
        OPT_INTEGER('t', "test", &run_config.test_cnt, "测试模式", NULL, 0, 0),
        OPT_INTEGER('s', "set-mpu-dtc", &run_config.set_dtc, "设置MPU故障 1-12", NULL, 0, 0),
        OPT_INTEGER('x', "set-rearview-type", &run_config.rearview_type, "设置后视镜类型  0为右镜 1为左镜", NULL, 0, 0),
        OPT_INTEGER('e', "clean-mpu-dtc", &run_config.clean_dtc, "清除MPU故障 1-12", NULL, 0, 0),
        OPT_INTEGER('E', "clean-nvm", &run_config.clean_nvm, "清除NVM分区 1:清除NVM DTC分区 2:清除NVM USER分区", NULL, 0, 0),
        OPT_INTEGER('g', "set-mcu-debug-level", &run_config.mcu_debug_level, 
            "设置MCU串口打印等级 5:DBG_DEBUG 4:DBG_INFO 3:DBG_SYS 2:DBG_WARNING 1:DBG_ERR", NULL, 0, 0),
        OPT_END(),
    };
    debug_init();

    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, 
            "\n本程序是读写MCU寄存器DEMO ", 
            NULL);

    argc = argparse_parse(&argparse, argc, argv);
    if(argc < 0)
        goto help;
    ret = other_argparse_parse(&run_config, argc, argv);
    if(ret)
        goto help;

    ret = RVMcu_Init();
    if(ret < 0){
        dbg_errfl("RVMcu_Init :%d",ret);
        return ret;
    }
    ret = run(&run_config);
    RVMcu_Exit();
    return ret;
help:
    argparse_help_cb_no_exit(&argparse, options);
    exit(1);
}