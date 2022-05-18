/**
 * @file server-select-TCP-UDP.c
 * 使用select同时处理TCP和UDP
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#define PORT 1226
int main(int argc, char **argv)
{
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0)
    {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in serveraddr;
    bzero(&serveraddr, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    const int on = 1;   // 一个开关
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if (bind(listenSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind");
        close(listenSocket);
        exit(-1);
    }

    if (listen(listenSocket, 200) < 0)
    {
        perror("listen");
        exit(-1);
    }

    int udpsocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpsocket < 0)
    {
        perror("socket");
        exit(-1);
    }

    if (bind(udpsocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind");
        close(udpsocket);
        close(listenSocket);
        exit(-1);
    }

    fd_set allset;
    FD_ZERO(&allset);
    while (1)
    {
        FD_SET(listenSocket, &allset);
        FD_SET(udpsocket, &allset);
        int maxfd = max(listenSocket, udpsocket);
        int readyNum;
        if ((readyNum = select(maxfd + 1, &allset, NULL, NULL, NULL)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("select");
                exit(-1);
            }
        }
        if (FD_ISSET(listenSocket, &allset))
        {
            int clientSocket = accept(listenSocket, NULL, NULL);
            int childpid = fork();
            if (childpid < 0)
            {
                perror("fork");
                exit(-1);
            }
            if (childpid == 0)
            {
                // tcp客户端业务
                close(listenSocket);
            }
            else
            {
                close(clientSocket);
            }
        }
        if (FD_ISSET(udpsocket, &allset))
        {
            // udp业务
        }
    }
}