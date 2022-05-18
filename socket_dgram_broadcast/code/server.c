#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "protosocket.h"

#define IPSTRSIZE 40
int main()
{
    // 取得socket
    int server_socket;
    server_socket = socket(AF_INET,SOCK_DGRAM,0);               // 0表示使用协议族里面默认的协议-IPPROTO_UDP
    if (server_socket < 0)
    {
        perror("socket()");
        exit(1);
    }
    // 设置socket属性,使其接收广播报
    int val = 1;
    int n = setsockopt(server_socket,SOL_SOCKET,SO_BROADCAST,&val,sizeof(val));
    if(n < 0)
    {
        perror("setsockopt()");
        exit(1);
    }

    // 给socket取地址
    struct sockaddr_in localaddr,clientaddr;
    socklen_t clientaddr_len;
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(atoi(SERVERPORT));
    // localaddr.sin_addr.s_addr要求是大整数,需要把ip地址做个转换
    inet_pton(AF_INET,"0.0.0.0",&localaddr.sin_addr.s_addr);
    
    int b = bind(server_socket,(void*)&localaddr,sizeof(localaddr));
    if(b < 0)
    {
        perror("bind()");
        exit(1);
    }


    // 接收消息
    clientaddr_len = sizeof(clientaddr);
    struct msg_st rbuf;
    memset(&rbuf,0,sizeof(rbuf));

    char ipstr[IPSTRSIZE];
    while (1)
    {
        recvfrom(server_socket,&rbuf,sizeof(rbuf),0,(void*)&clientaddr,&clientaddr_len);
        // 大整数转换成点分式
        inet_ntop(AF_INET,&clientaddr.sin_addr,ipstr,IPSTRSIZE);
        printf("server: %s:%d ---\n", \
        ipstr,ntohs(clientaddr.sin_port));
        printf("NAME = %s\n",rbuf.name);
        printf("MATH = %d\n",ntohl(rbuf.math));
        printf("CHINESE = %d\n",ntohl(rbuf.chinese));
    }
    

    // 关闭socket
    close(server_socket);

    exit(0);
}