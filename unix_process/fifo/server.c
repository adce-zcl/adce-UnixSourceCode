/**
 * @file server.c
 * 可以处理多个客户端的通信
 * 使用结构体消息
 * 有一个众所周知的server fifo，服务端从这里面读取数据
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "proto.h"
#include <signal.h>
void handle(int p)
{
    printf("logout.\n");
    exit(0);
}
int main(int agrc, char **argv)
{
    signal(SIGINT, handle);
    // 尝试创建并打开fifo
    if (mkfifo(SERVER_FIFO, FIFO_MODE) < 0 && errno != EEXIST)
    {
        perror("mkfifo");
        exit(-1);
    }
    // 自己先打开写fifo，再打开读fifo，防止客户端关闭的时候，服务器整个被阻塞
    int readfd = open(SERVER_FIFO, O_RDONLY);
    if (readfd < 0)
    {
        perror("open");
        exit(-1);
    }
    // 这个只有在readfd阻塞之后才会执行，readfd只会阻塞一次，防止readfd失效，一直占用写端，但是这个fd不用，用nofd表示。
    int nofd = open(SERVER_FIFO, O_WRONLY); 
    if (nofd < 0)
    {
        perror("open");
        exit(-1);
    }

    // 死循环执行任务
    char buf[1024];
    char client_path[1024];
    struct msg_st msg;
    printf("server begin recv:\n");
    while (read(readfd, &msg, sizeof(msg)) > 0)
    {
        printf("[%d]:%s\n", msg.pid, msg.data); // 服务器把接受到的消息打印到终端上
        sprintf(client_path, "%s%d", CLIENT_FIFO, msg.pid);
        sprintf(buf, "[%d]:OK.", msg.pid);
        int sendfd = open(client_path, O_WRONLY);
        if (sendfd < 0)
        {
            perror("open");
            exit(-1);
        }
        strncpy(msg.data, buf, sizeof(buf));
        write(sendfd, &msg, sizeof(msg));
    }
    exit(0);
}