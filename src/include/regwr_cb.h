/**
 * @file regwr_cb.h
 * @brief 使用寄存器读写的形式实现的环形缓冲区，使MCU和MPU数据流畅通无阻
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-09-02
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

#ifndef _REGWR_CB_H_
#define _REGWR_CB_H_


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define CBREG_CMD_GET_SIZE         0x00         /* 读4个字节 获取已用容量 */
#define CBREG_CMD_GET_FREESIZE     0x01         /* 读4个字节 获取可用容量 */
#define CBREG_CMD_READ             0x02         /* 读N个字节 调用read函数 读前先获取容量 超过将失败 */
#define CBREG_CMD_WRITE            0x03         /* 写N个字节 调用write函数 写前先获取容量 超过将失败 */
#define CBREG_CMD_CLEAN            0x04         /* 随便写1个字节 调用clean函数 */
#define CBREG_CMD_READAIR          0x05         /* 读空气 写4个字节 写要读的空气数量 调用 crb_ReadAir */
#define CBREG_CMD_PEEP             0x06         /* 偷看N个字节 调用read函数 读前先获取容量 超过将失败 */
#define CBREG_SIZE                 0x07         /* CBREG 寄存器的长度 */

typedef struct _RegWrCbHandle{
    /**
    * @brief  write_reg 和 read_reg
    * @param  addr             寄存器地址
    * @param  data             寄存器数据或缓冲区空间
    * @param  data_len         读的大小或者写的大小
    * @return int              失败返回负数 成功返回0 
    */
    int (*write_reg)(uint16_t addr, const uint8_t *data, uint16_t data_len, uint32_t timeout);
    int (*read_reg)(uint16_t addr, uint8_t *data, uint16_t data_len, uint32_t timeout);
}RegWrCbHandle;

extern int RegWrCb_Size(RegWrCbHandle *h, uint16_t cb_addr, uint32_t timeout);
extern int RegWrCb_FreeSize(RegWrCbHandle *h, uint16_t cb_addr, uint32_t timeout);
extern int RegWrCb_Read(RegWrCbHandle *h, uint16_t cb_addr, uint8_t *buf, uint32_t buf_size, uint32_t timeout);
extern int RegWrCb_Write(RegWrCbHandle *h, uint16_t cb_addr, const uint8_t *data, uint32_t data_size, uint32_t timeout);
extern int RegWrCb_Clean(RegWrCbHandle *h, uint16_t cb_addr, uint32_t timeout);
extern int RegWrCb_ReadAir(RegWrCbHandle *h, uint16_t cb_addr, uint32_t read_size, uint32_t timeout);
extern int RegWrCb_Peep(RegWrCbHandle *h, uint16_t cb_addr, uint8_t *buf, uint32_t buf_size, uint32_t timeout);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _REGWR_CB_H_
