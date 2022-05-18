#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include "protosocket.h"

// 创建多播组

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
    struct ip_mreqn mreq;
    // 将多播组的地址转换成大整数
    inet_pton(AF_INET,MTROUP,&mreq.imr_multiaddr);
    // 将本机地址转换成大整数
    inet_pton(AF_INET,"0.0.0.0",&mreq.imr_address);
    // 将网卡设备转换成标号
    unsigned int index = if_nametoindex("eth33");
    mreq.imr_ifindex = index;

    int n = setsockopt(clientsocket,IPPROTO_IP,IP_MULTICAST_IF,&mreq,sizeof(mreq));
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
    inet_pton(AF_INET,MTROUP,&serveraddr.sin_addr);
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