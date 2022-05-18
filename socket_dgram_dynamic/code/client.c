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
    if(argc < 3)
    {
        fprintf(stderr,"Usage..\n");
        exit(1);
    }

    // 判断名字的大小值
    if(strlen(argv[2]) > NAMEMAX)
    {
        fprintf(stderr,"Name is too long!\n");
        exit(1);
    }

    // 创建socket
    int clientsocket;
    clientsocket = socket(AF_INET,SOCK_DGRAM,0);
    if(clientsocket < 0)
    {
        perror("socket()");
        exit(0);
    }

    // 创建动态报文
    struct msg_st *sbufp = NULL;
    int size = sizeof(struct msg_st) + strlen(argv[2]);
    sbufp = malloc(size);
    if(sbufp == NULL)
    {
        perror("malloc()");
        exit(1);
    }

    // 报文结构体赋值
    strcpy(sbufp->name,argv[2]);
    sbufp->math = htonl(rand()%100);
    sbufp->chinese = htonl(rand()%100);

    // 创建server端地址
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET,argv[1],&serveraddr.sin_addr);

    // 发送报文
    ssize_t num = sendto(clientsocket,sbufp,size,0,(void*)&serveraddr,sizeof(serveraddr));
    if(num < 0)
    {
        perror("sendto()");
        exit(0);
    }
    puts("OK!");
    close(clientsocket);
    exit(0);
}