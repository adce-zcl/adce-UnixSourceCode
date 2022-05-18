/**
 * @file proto.h
 * 客户端和服务端共同遵守的协议
 * 定义消息体数据结构
 * 为了简化，这里服务器就统一回复：[pid] OK.
 */
#ifndef PROTO_H_
#define PROTO_H_

#include <limits.h>
#include <unistd.h>
#include <string.h>
#define FIFO_MODE (S_IRWXU | S_IRWXG | S_IRWXO)
#define SERVER_FIFO "/home/adce/mycode/vscode/unix_process/fifo/file/serverfifo"
#define CLIENT_FIFO "/home/adce/mycode/vscode/unix_process/fifo/file/clientfifo."
#define MEGDATESIZE (PIPE_BUF - sizeof(int))
struct msg_st
{
    int pid;        // > 0
    char data[MEGDATESIZE];  // date
};
#endif