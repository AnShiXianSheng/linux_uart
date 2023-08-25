/**
 * @file crc_check.c
 * @brief crc校验
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-08-16
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */


#include <stdint.h>
#include "crc_check.h"

static const uint16_t crcTalbeAbs[] = {
	0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401, 
	0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400, 
};

uint16_t crc16(uint16_t init_val,const uint8_t* msg, uint16_t msg_len)
{
    uint16_t i;
    uint8_t  ch;
	uint16_t crc_val = init_val;
    for (i = 0; i < msg_len; i++)
    {
        ch = *msg++;
        crc_val = crcTalbeAbs[(ch ^ crc_val) & 15] ^ (crc_val >> 4);
        crc_val = crcTalbeAbs[((ch >> 4) ^ crc_val) & 15] ^ (crc_val >> 4);
    }
    return crc_val;
}
