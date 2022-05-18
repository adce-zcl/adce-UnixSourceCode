/**
 * @file client.c
 * 客户端有自己独立的client fifo
 * fifo的地址通过结构体传送给服务器
 * 服务器通过这个fifo传回数据
 */
#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
void handle(int p)
{
    printf("logout.\n");
    exit(0);
}
int main(int argc, char **argv)
{
    signal(SIGINT, handle);
    pid_t pid = getpid();
    char path[1024];
    sprintf(path, "%s%d", CLIENT_FIFO, pid);
    if (mkfifo(path, FIFO_MODE) < 0 && errno != EEXIST) // 新建自己的fifo
    {
        perror("mkfifo");
        exit(-1);
    }
    // 打开服务端的fifo
    int sendfd = open(SERVER_FIFO, O_WRONLY);
    struct msg_st msg;
    char buf[1024];
    gets(buf);  // 手动输入消息
    msg.pid = pid;
    strncpy(msg.data, buf, strlen(buf));
    write(sendfd, &msg, sizeof(msg));   // 发送消息
    int recvfd = open(path, O_RDONLY);  // 打开自己的fifo，读内容，注意，不能打开早了，否则会阻塞
    while(1)
    {
        read(recvfd, &msg, sizeof(msg));
        puts(msg.data);
        gets(buf);
        strncpy(msg.data, buf, strlen(buf));
        write(sendfd, &msg, sizeof(msg));   // 发送消息
    }
    exit(0);
}