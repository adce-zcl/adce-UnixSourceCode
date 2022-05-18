#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "proto.h"
// 因为http1.1有请求头
// 为了方便，使用http1.0 请求图片
#define BUFSIZE 1024
int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Usage...\n");
        exit(1);
    }

    // 建立socket
    int clientsocket;
    clientsocket = socket(AF_INET,SOCK_STREAM,0);
    if(clientsocket < 0)
    {
        perror("socket()");
        exit(1);
    }

    // 请求连接
    struct sockaddr_in serveradd;
    serveradd.sin_family = AF_INET;
    serveradd.sin_port = htons(80);                 // 上网端口：80
    inet_pton(AF_INET,argv[1],&serveradd.sin_addr);
    int c = connect(clientsocket,(void*)&serveradd,sizeof(serveradd));
    if(c < 0)
    {
        perror("connect()");
        exit(1);
    }

    FILE *fp = fdopen(clientsocket,"r+");
    if(fp == NULL)
    {
        perror("fdopen()");
        exit(1);
    }

    fprintf(fp,"GET /test.png\r\n\r\n");        // 发请求给浏览器
    fflush(fp);

    char rbuf[BUFSIZE];
    while (1)
    {
        int len = fread(rbuf,1,BUFSIZE,fp);               // 从浏览器中读内容到rbuf
        if(len <= 0)
        {
            break;
        }
        fwrite(rbuf,1,len,stdout);                        // 从rbuf中一次读一个字节到标准输出
    }
    
    
    fclose(fp);

    // 收发消息 rcve()--close()--

    exit(0);
}