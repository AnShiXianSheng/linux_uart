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


    if(config->is_write || config->is_read){
        if(argc != 2) return -1;
        config->reg_addr = strtol(argv[0], &endptr, 0);
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
        config->mode = FUN_RW;
        return 0;
    }

    if(config->is_show_mcu_info){
        config->mode = FUN_SHOW_MCU_INFO;
        return 0;
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
        .is_look_dtc = 0,
        .mode = FUN_MPU_ONLINE
    };
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("基本命令"),
        OPT_BOOLEAN('w', "write", &run_config.is_write, "写寄存器", NULL, 0, 0),
        OPT_BOOLEAN('r', "read", &run_config.is_read, "读寄存器", NULL, 0, 0),
        OPT_BOOLEAN('c', "read-can-event", &run_config.is_read_can_event, "读所有CAN事件", NULL, 0, 0),
        OPT_BOOLEAN('i', "show-mcu-info", &run_config.is_show_mcu_info, "显示mcu所有信息", NULL, 0, 0),
        OPT_BOOLEAN('l', "look-mpu-dtc", &run_config.is_look_dtc, "显示MPU故障", NULL, 0, 0),
        OPT_STRING('u', "update", &run_config.mcu_firmware, "升级固件", NULL, 0, 0),
        OPT_STRING('U', "Update", &run_config.mcu_force_firmware, "强行升级固件", NULL, 0, 0),
        OPT_INTEGER('t', "test", &run_config.test_cnt, "测试模式", NULL, 0, 0),
        OPT_INTEGER('s', "set-mpu-dtc", &run_config.set_dtc, "设置MPU故障 1-12", NULL, 0, 0),
        OPT_INTEGER('e', "clean-mpu-dtc", &run_config.clean_dtc, "清除MPU故障 1-12", NULL, 0, 0),
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