#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "protosocket.h"

int main(int argc, char **argv)
{
    // 创建socket
    int clientsocket;
    clientsocket = socket(AF_INET,SOCK_DGRAM,0);
    if(clientsocket < 0)
    {
        perror("socket()");
        exit(0);
    }
    // 设置socket属性
    // SOL_SOCKET -- 表示socket属于SOL_SOCKET层
    int val = 1;                    // 表示打开
    int n = setsockopt(clientsocket,SOL_SOCKET,SO_BROADCAST,&val,sizeof(val));
    if(n < 0)
    {
        perror("sersockopt()");
        exit(1);
    }

    struct msg_st sbuf;
    memset(&sbuf,0,sizeof(sbuf));
    strcpy(sbuf.name,"张三");
    sbuf.math = htonl(99);
    sbuf.chinese = htonl(100);
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET,"255.255.255.255",&serveraddr.sin_addr);
    ssize_t num = sendto(clientsocket,&sbuf,sizeof(sbuf),0,(void*)&serveraddr,sizeof(serveraddr));
    if(num < 0)
    {
        perror("sendto()");
        exit(0);
    }
    puts("OK!");
    close(clientsocket);
    exit(0);
}