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
    // 创建socket
    int server_socket;
    server_socket = socket(AF_INET,SOCK_DGRAM,0);               // 0表示使用协议族里面默认的协议-IPPROTO_UDP
    if (server_socket < 0)
    {
        perror("socket()");
        exit(1);
    }
    
    // 创建本机地址
    struct sockaddr_in localaddr,clientaddr;
    socklen_t clientaddr_len;
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(atoi(SERVERPORT));
    // localaddr.sin_addr.s_addr要求是大整数,需要把ip地址做个转换
    inet_pton(AF_INET,"127.0.0.1",&localaddr.sin_addr.s_addr);
    
    // 绑定本机地址
    int b = bind(server_socket,(void*)&localaddr,sizeof(localaddr));
    if(b < 0)
    {
        perror("bind()");
        exit(1);
    }


    // 接收消息
    clientaddr_len = sizeof(clientaddr);
    struct msg_st *rbufp =NULL;
    int size = sizeof(struct msg_st) + NAMEMAX - 1;
    rbufp = malloc(size);
    if(rbufp == NULL)
    {
        perror("malloc()");
        exit(1);
    }

    char ipstr[IPSTRSIZE];
    while (1)
    {
        recvfrom(server_socket,rbufp,size,0,(void*)&clientaddr,&clientaddr_len);
        // 大整数转换成点分式
        inet_ntop(AF_INET,&clientaddr.sin_addr,ipstr,IPSTRSIZE);
        printf("server: %s:%d ---\n", \
        ipstr,ntohs(clientaddr.sin_port));
        printf("NAME = %s\n",rbufp->name);
        printf("MATH = %d\n",ntohl(rbufp->math));
        printf("CHINESE = %d\n",ntohl(rbufp->chinese));
    }
    

    // 关闭socket
    close(server_socket);

    exit(0);
}