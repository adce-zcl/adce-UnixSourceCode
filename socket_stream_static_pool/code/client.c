#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "proto.h"

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
    serveradd.sin_port = htons(atoi(SERVERPORT));
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

    long long stamp;
    if(fscanf(fp,FMT_STAMP,&stamp) < 1)
    {
        fprintf(stderr,"Bad format!\n");
        exit(1);
    }
    else
    {
        fprintf(stdout,"stamp = %lld\n",stamp);
    }
    fclose(fp);

    // 收发消息 rcve()--close()--

    exit(0);
}