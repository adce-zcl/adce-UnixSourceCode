/**
 * @file roto.h
 * @author your name (you@domain.com)
 * @brief 自定义通信协议
 * @version 0.1
 * @date 2022-02-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _PROTO_H_
#define _PROTO_H_
#define KEYPATH "/etc/services"
#define KEYPROJ 'a'                 // 表示一个整形数
#define NAMESIZE 1024
struct msg_st
{
    char name[NAMESIZE];
    int math;
    int chinese;
    long mtype;         // must > 0
};

#endif