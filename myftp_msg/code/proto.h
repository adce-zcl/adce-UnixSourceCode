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
#ifndef _PROTO_H_
#define _PROTO_H_
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

typedef struct msg_data_st      // C端接收的包
{
    long mtype;             // must be MSG_DATA
    char data[DATAMAX];
    long datalen;           // data包中的有效长度
}msg_data_t;

typedef struct msg_eot_st       // C端接收的包
{
    long mtype;             // must be MSG_EOT
}msg_eot_t;

union msg_s2c_un            // 两者之间的共用体，因为C端收到的包一定要么是data包，要么是eot包
{
    long mtype;
    msg_data_t datamsg;
    msg_eot_t eotmsg;
};




#endif