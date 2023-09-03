/**
 * @file spi_reg.c
 * @brief spi 读寄存器实现
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-08-16
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "crc_check.h"
#include "memctrl.h"
#include "spi_reg.h"
#include "debug.h"
#include "pp_uart.h"

#define SPI_TIMEOUT_MS                  100
#define SPI_START_TIMEOUT_MS            55


#define SPI_CMD_READ_REG                (0x03)
#define SPI_CMD_WRITE_REG               (0x06)
#define SPI_ACK                         'A'
#define SPI_NACK                        'N'
#define SPI_CMD_START                   'S'
#define SPI_DATA_START                  'D'

/* 可用的总命令长度，没算'S' */
#define SPI_CMD_LEN                         8

/* 这里的命令偏移没计算 启动字节 START */
#define CMD_1BYTE_OFFSET                    0
#define CMD_WR_ADDR_2BYTE_OFFSET            1
#define CMD_WR_LEN_2BYTE_OFFSET             3
#define CMD_WR_CMD_LEN                      5

#define WR_ACK_1BYTE_OFFSET                 0
/* 这里的数据偏移没计算ACK字节 */
#define WR_ACK_DATA_XBYTE_OFFSET            0
#define WR_ACK_LEN                          1 
#define WR_CRC_LEN                          2 
#define WR_DATA_ALIGN_BYTE                  8  /* 传输数据时向8字节取整 */


#define UART_SPEED                      1000000



static int _TransferSpi(SpiRegHandle *h, size_t length)
{
    int ret;
	struct spi_ioc_transfer transfer = {0};
	transfer.rx_buf = (unsigned long)h->rx_buf;
	transfer.tx_buf = (unsigned long)h->tx_buf;
	transfer.len = length;

    ret = ioctl(h->fd, SPI_IOC_MESSAGE(1), &transfer);
	return ret;
}

static int _GotoStartCmd(SpiRegHandle *h, uint32_t timeout){
    uint8_t ch = SPI_CMD_START;
    int ret;
    uart_InClean(h->uart_fd);
    ret = uart_Write(h->uart_fd, &ch, 1);
    if(ret != 1) return -1;
    ret = uart_Read(h->uart_fd, &ch, 1, (int)timeout);
    //if(ret <= 0 || ch != SPI_ACK) return -1;
    if(ret <= 0) return -1;
    if(ch != SPI_ACK) {
        //dbg_debugfl("ret = %d ch = 0x%02x %c",ret ,ch ,ch);
        return -1;
    }
    uart_InClean(h->uart_fd);
    return 0;
}

static int _WaitAck(SpiRegHandle *h, uint32_t timeout){
    uint8_t ch = 0x00;
    int ret;
    ret = uart_Read(h->uart_fd, &ch, 1, (int)timeout);
    if(ret != 1 || ch != SPI_ACK) return -1;
    uart_InClean(h->uart_fd);
    return 0;
}



/**
 * @brief 读spi寄存器
 * @param  h                句柄
 * @param  reg_addr         寄存器地址
 * @param  reg_cnt          要读的寄存器数量
 * @param  reg_data         装寄存器数据的指针
 * @return return 成功0 失败负数 一般情况下 -2是超时 -3是crc错误，不排除其他系统返回值和他们一样
 */
int SpiReg_Read(SpiRegHandle *h, uint16_t reg_addr, uint16_t reg_cnt, uint8_t *reg_data, uint32_t timeout){
    int ret;
    uint16_t crc16_val = 0xffff;
    uint16_t read_crc16_val = 0x0000;
    size_t trans_length = 0;
    uint32_t fill_len; 

    if(h == NULL) return -1;
    memset(h->rx_buf, 0xff, SPI_RT_MSG_MAX_SIZE);
    memset(h->tx_buf, 0xff, SPI_RT_MSG_MAX_SIZE);
    SET_MEM_VAL_TYPE_SYSTEM_TO_BIG(h->tx_buf + CMD_1BYTE_OFFSET, SPI_CMD_READ_REG, uint8_t);
    SET_MEM_VAL_TYPE_SYSTEM_TO_BIG(h->tx_buf + CMD_WR_ADDR_2BYTE_OFFSET, reg_addr, uint16_t);
    SET_MEM_VAL_TYPE_SYSTEM_TO_BIG(h->tx_buf + CMD_WR_LEN_2BYTE_OFFSET, reg_cnt, uint16_t);
    crc16_val = crc16(crc16_val, h->tx_buf, CMD_WR_CMD_LEN);

    ret = _GotoStartCmd(h, timeout);
    if(ret < 0) return -2;
    ret = _TransferSpi(h, SPI_CMD_LEN);
    if(ret < 0) return ret;
    ret = _WaitAck(h, timeout);
    if(ret < 0) return -2;
    trans_length = reg_cnt + WR_CRC_LEN;
        /* 计算需要填充的大小 */
    fill_len =  (WR_DATA_ALIGN_BYTE - trans_length%WR_DATA_ALIGN_BYTE) % WR_DATA_ALIGN_BYTE;
    trans_length += fill_len;
    ret = _TransferSpi(h, trans_length);
    if(ret < 0) return ret;

    ret = _WaitAck(h, timeout);
    if(ret < 0) return -2;
    
    crc16_val = crc16(crc16_val, h->rx_buf, reg_cnt);

    SET_MEM_VAL_TYPE_BIG_TO_SYSTEM(&read_crc16_val, 
        GET_MEM_VAL(h->rx_buf+reg_cnt, uint16_t), 
        uint16_t);
    //dbg_infohex(h->rx_buf, reg_cnt+WR_CRC_LEN);
    if(read_crc16_val != crc16_val){
        return -3;
    }
    memcpy(reg_data, h->rx_buf, reg_cnt);
    return 0;
}

int SpiReg_Write(SpiRegHandle *h, uint16_t reg_addr, uint16_t reg_cnt, const uint8_t *reg_data, uint32_t timeout){
    int ret;
    uint16_t crc16_val = 0xffff;
    size_t trans_length = 0;
    uint32_t fill_len; 

    if(h == NULL) return -1;
    memset(h->rx_buf, 0xff, SPI_RT_MSG_MAX_SIZE);
    memset(h->tx_buf, 0xff, SPI_RT_MSG_MAX_SIZE);
    SET_MEM_VAL_TYPE_SYSTEM_TO_BIG(h->tx_buf + CMD_1BYTE_OFFSET, SPI_CMD_WRITE_REG, uint8_t);
    SET_MEM_VAL_TYPE_SYSTEM_TO_BIG(h->tx_buf + CMD_WR_ADDR_2BYTE_OFFSET, reg_addr, uint16_t);
    SET_MEM_VAL_TYPE_SYSTEM_TO_BIG(h->tx_buf + CMD_WR_LEN_2BYTE_OFFSET, reg_cnt, uint16_t);
    crc16_val = crc16(crc16_val, h->tx_buf, CMD_WR_CMD_LEN);

    ret = _GotoStartCmd(h, timeout);
    if(ret < 0) return -2;

    ret = _TransferSpi(h, SPI_CMD_LEN);
    if(ret < 0) return ret;

    ret = _WaitAck(h, timeout);
    if(ret < 0) return -2;
    /* 开始准备发送的数据 */
    crc16_val = crc16(crc16_val, reg_data, reg_cnt);
    memcpy(h->tx_buf, reg_data, reg_cnt);
    SET_MEM_VAL_TYPE_SYSTEM_TO_BIG(h->tx_buf+reg_cnt, crc16_val, uint16_t);
    trans_length = reg_cnt + WR_CRC_LEN;
    /* 计算需要填充的大小 */
    fill_len =  (WR_DATA_ALIGN_BYTE - trans_length%WR_DATA_ALIGN_BYTE) % WR_DATA_ALIGN_BYTE;
    memset(h->tx_buf+trans_length, 0xff, fill_len);
    trans_length += fill_len;
    //dbg_infohex(h->tx_buf, trans_length);
    ret = _TransferSpi(h, trans_length);
    if(ret < 0) return ret;

    ret = _WaitAck(h, timeout);
    if(ret < 0) return -2;
    return ret;
}

/**
 * @brief open spi 配置为既定频率后返回文件描述符
 * @param  dev              设备节点 /dev/ttyLP1
 * @param  speed            spi时钟频率
 * @return int 
 */
int SpiReg_Init(SpiRegHandle *h, char* spi_dev, char* uart_dev, uint32_t spi_speed){
    int fd,ret;
    uint8_t mode = SPI_MODE_3;              // 设置模式为 0
    uint8_t lsb_first = 0;                  // 设置为 MSB 优先
    uint8_t bits_per_word = 8;              // 设置数据位数为 8 位

    fd = open(spi_dev, O_RDWR);  // 打开 SPI 设备文件
    if (fd < 0)
        return fd;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if ( ret < 0) goto err_out;
    
    ret = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb_first);
    if ( ret < 0) goto err_out;

    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
    if ( ret < 0) goto err_out;

    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if ( ret < 0) goto err_out;
    
    memset(h,0,sizeof(SpiRegHandle));
    h->fd =fd;
    h->speed = spi_speed;

    h->uart_fd = uart_Open(uart_dev, UART_SPEED, 8, 1, 'N');
    if(h->uart_fd < 0) goto err_out;
    return 0;
err_out:
    close(fd);
    return ret;
}

void SpiReg_Exit(SpiRegHandle *h){
    close(h->fd);
}
