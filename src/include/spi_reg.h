/**
 * @file spi_reg.h
 * @brief 
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-08-16
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */
 #ifndef _SPI_REG_H_
 #define _SPI_REG_H_

#define SPI_RT_MSG_MAX_SIZE 1024

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef struct _SpiRegHandle{
    int fd;
    int uart_fd;
    uint8_t tx_buf[SPI_RT_MSG_MAX_SIZE];
    uint8_t rx_buf[SPI_RT_MSG_MAX_SIZE];
    uint32_t speed;
}SpiRegHandle;



extern int SpiReg_Write(SpiRegHandle *h, uint16_t reg_addr, uint16_t reg_cnt, const uint8_t *reg_data, uint32_t timeout);
extern int SpiReg_Read(SpiRegHandle *h, uint16_t reg_addr, uint16_t reg_cnt, uint8_t *reg_data, uint32_t timeout);
extern int SpiReg_Init(SpiRegHandle *h, char* spi_dev, char* uart_dev, uint32_t speed);
extern void SpiReg_Exit(SpiRegHandle *h);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
 
 
 #endif // _SPI_REG_H_