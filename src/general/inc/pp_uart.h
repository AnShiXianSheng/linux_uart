/**
 * @file pp_uart.h
 * @brief 
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2022-10-18
 * 
 * @copyright Copyright (c) 2022  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 *      2022/10/18 14:49 +8 创建文件
 */
#ifndef __PP_UART_H__
#define __PP_UART_H__


#include <sys/types.h> 

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern int uart_Open(const char *uart_name, int speed, int databits, int stopbits, int parity);
extern void uart_Close(int fd);
extern ssize_t uart_Write(int fd, void *data, size_t data_len);
extern ssize_t uart_Read(int fd,void *data_buf, size_t buf_size, int timeout);
extern void uart_InClean(int fd);
extern void uart_OutClean(int fd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /*__PP_UART_H__ */
