/**
 * @file server-select.c
 * 单进程，使用select实现多个已经连接套接字的通信
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 1227
#define ADDRESS "127.0.0.1"


int main(int argc, char **argv)
{
    int listenSocket;

    // socket
    listenSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listenSocket < 0)
    {
        perror("listen");
        exit(-1);
    }

    struct sockaddr_in serveraddr;
    bzero(&serveraddr, 0);  // 初始化,好习惯
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDRESS, &serveraddr.sin_addr.s_addr);
    
    // bind
    if (bind(listenSocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind");
        exit(-1);
    }

    // listen
    listen(listenSocket, 200);

    int clients[FD_SETSIZE], clientsSize;
    int maxfd = listenSocket;
    
    // 数组初始化
    for (int i = 0; i < FD_SETSIZE; ++i)
    {
        // 初始化-1
        clients[i] = -1;
    }

    fd_set allset;
    FD_ZERO(&allset);
    FD_SET(listenSocket, &allset);

    // 循环事物
    while (1)
    {
        fd_set curset = allset;
        int maxi;
        int readyNum = select(maxfd + 1, &curset, NULL, NULL, NULL);

        // 如果是listensocket可读，说明有连接
        if (FD_ISSET(listenSocket, &curset))
        {
            printf("have new connect.\n");
            int clientSocket;
            struct sockaddr_in clientaddr;
            bzero(&clientaddr, 0);
            socklen_t clientaddrlen = sizeof(clientaddr);

            clientSocket = accept(listenSocket, (struct sockaddr *)&clientaddr, &clientaddrlen);
            if (clientSocket < 0)
            {
                perror("accept");
                exit(-1);
            }

            // 把这个通信套接字保存在第一个空位数组里
            int i;
            for (i = 0; i < FD_SETSIZE; ++i)
            {
                if (clients[i] == -1)
                {
                    clients[i] = clientSocket;
                    break;
                }
            }
            if (i == FD_SETSIZE)    // 没有空位置了
            {
                fprintf(stderr, "too many clients.\n");
                close(clientSocket);
                // continue;  不能continue，因为可能还有别的通信套接字要处理
            }
            if (i > maxi)
            {
                maxi = i;   // 数组的最大位置，用来缩减遍历
            }

            // 把这个客户端套接字加入到文件描述符集合里
            FD_SET(clientSocket, &allset);
            if (clientSocket > maxfd)   // 更新最大文件描述符
            {
                maxfd = clientSocket;
            }

            --readyNum;
            if (readyNum == 0)
            {
                continue;
            }
        }

        // readyNum != 0, 继续处理活动的通信套接字
        int sockfd;
        char buf[1024];
        for (int i = 0; i <= maxi; ++i)
        {
            if (clients[i] < 0)
            {
                continue;
            }
            sockfd = clients[i];
            if (FD_ISSET(sockfd, &curset))  // 如果这个套接字有活动
            {
                int n = read(sockfd, buf, 1024);
                if (n < 0)
                {
                    perror("read");
                    exit(-1);
                }
                else if (n == 0)    // 注意，这里0只能表示文件结束，对于套接字，就是对端关闭了套接字，发送EOF
                {
                    close(sockfd);
                    printf("have socket close.\n");
                    clients[i] = -1;
                    FD_CLR(sockfd, &allset);
                }
                else
                {
                    printf("[%d]:%s.\n", sockfd, buf);
                    write(sockfd, buf, n);
                }
                --readyNum; // 所有的活动套接字都处理完了
                if (readyNum == 0)
                {
                    break;
                }
            }
        }
    }
}