/**
 * @file proto.h
 * @author your name (you@domain.com)
 * @brief 报式套接字
 * 报式套接字不需要长连接,所以在发送和接收的时候需要表明双者身份
 * 流式套接字因为建立长连接,所以不需要表明双方身份
 * 
 * 动态报式套接字
 * 使用可以变长的报
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
struct msg_st
{
    uint32_t math;       
    uint32_t chinese;
    uint8_t name[NAMEMAX];                     // 起一个占位的作用
    // 最好是吧变长的数组，放在结构体的最后面，不然会影响其他成员的值
}__attribute__((packed));                       // 防止自动对齐



#endif