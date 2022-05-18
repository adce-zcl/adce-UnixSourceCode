/**
 * @file proto.h
 * @author your name (you@domain.com)
 * @brief 报式套接字
 * 报式套接字不需要长连接,所以在发送和接收的时候需要表明双者身份
 * 流式套接字因为建立长连接,所以不需要表明双方身份
 * 
 * 使用udp完成组播
 * 
 * @version 0.1
 * @date 2022-02-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef PROTOSOCKET_H_
#define PROTOSOCKET_H_

#include <stdint.h>
#define NAMEMAX 512-8-8                         // 推荐一个报文512-8（报头）-8（两个uint32_t字节）
#define SERVERPORT "1989"
#define CLIENTPORT "1988"

#define MTROUP "224.2.2.2"                      // 组号
struct msg_st
{
    uint8_t name[NAMEMAX];                     // char *name没有用，因为传指针过去，内存过不去
    uint32_t math;       
    uint32_t chinese;
    
}__attribute__((packed));                       // 防止自动对齐



#endif