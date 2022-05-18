/**
 * @file roto.h
 * @author your name (you@domain.com)
 * @brief 自定义通信协议
 * 完成简易的ftp，可以使用状态机
 * @version 0.1
 * @date 2022-02-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _PROTO2_H_
#define _PROTO2_H_
#define KEYPATH "/etc/services"     // 这两个都是用来获得唯一的key值的
#define KEYPROJ 'a'                 // 表示一个整形数
#define PATHMAX 1024
#define DATAMAX 1024
enum                    // 包的种类
{
    MSG_PATH = 1,
    MSG_DATA,
    MSG_EOT
};
typedef struct msg_path_st      // S端接收的包
{
    long mtype;                 /*must be MSG_PATH*/
    char path[PATHMAX];         /*ASCIIZ带尾零的串*/
}msg_path_t;

typedef struct msg_s2c_st   // 可以单独使用mtype或者datalen来确定是data包还是eot包
{
    long mtype;             // must be MSG_DATA
    /*
    * datalen > 0 :Data
    * datalen ==0 :eot
    */
    long datalen;           // data包中的有效长度

    char data[DATAMAX];
};








#endif