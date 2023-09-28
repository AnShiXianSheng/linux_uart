/**
 * @file can-msg.h
 * @brief 定义can消息报文结构体
 * @author simon.xiaoapeng (simon.xiaoapeng@gmail.com)
 * @version 1.0
 * @date 2023-09-06
 * 
 * @copyright Copyright (c) 2023  simon.xiaoapeng@gmail.com
 * 
 * @par 修改日志:
 */

#ifndef _CAN_MSG_H_
#define _CAN_MSG_H_

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#define PCAN_FLAG_TX_RECORD_CNT     0x01

/* 是否进行发送次数的记录 */
#define PCan_IsRecotdSendCnt(can_msg_ptr)    ((can_msg_ptr)->flag & PCAN_FLAG_TX_RECORD_CNT)

typedef  struct _PCanMsg{
    uint32_t    can_id;         
    uint8_t     can_data[8];    
    uint16_t    can_time;       /* can发送或者接收的时的时间 */
    uint8_t     can_len;        
    uint8_t     flag;
}PCanMsg;

typedef enum PCanTxSta{
    PCTS_SEND_SUCCEED,          /* 发送成功 */
    PCTS_HANG_UP,               /* 挂起中，等待发送 */
    PCTS_SENDING,               /* 正在发送 */
    PCTS_SEND_ERROR,            /* 发送错误 */
}PCanTxSta;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif // _CAN-MSG_H_

