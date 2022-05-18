/**
 * @file server-poll.c
 * 使用poll，只需要维护一个结构体数组，不需要维护两个fd_set和一个clients数组
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/types.h>
#include <limits.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <time.h>
#include <errno.h>
#define ADDRESS "127.0.0.1"
#define PORT 1227
int main(int argc, char **argv)
{
    int listenSocket;
    // listen
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0)
    {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in serveraddr;
    bzero(&serveraddr, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDRESS, &serveraddr.sin_addr.s_addr);

    // bind
    if (bind(listenSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind");
        exit(-1);
    }

    // listen
    if (listen(listenSocket, 200) < 0)
    {
        perror("listen");
        exit(-1);
    }

    // 初始化客户端数组
    struct pollfd clients[1024];
    clients[0].fd = listenSocket;
    clients[0].events = POLLRDNORM; // 注册这个套接字的读事件
    for (int i = 1; i < 1024; ++i)
    {
        clients[i].fd = -1;     // poll默认对-1的文件描述符不处理
    }
    int maxi = 0;   // 规定最大的有效数组长度

    // 开始事物
    while (1)
    {
        int readyNum = poll(clients, maxi + 1, -1); // -1永久等待
        // 如果是监听套接字可读，说明有客户连接
        if(clients[0].revents & POLLRDNORM)
        {
            printf("have client connect.\n");
            int clientSocket;
            clientSocket = accept(listenSocket, NULL, NULL);
            if (clientSocket < 0)
            {
                perror("accept");
                exit(-1);
            }

            // 找出最后一个空闲的数组，把clientsocket放入监听
            int i;
            for ( i = 1; i < 1024; ++i)
            {
                if(clients[i].fd == -1)
                {
                    clients[i].fd = clientSocket;
                    clients[i].events = POLLRDNORM;
                    break;
                }
            }
            if (i == 1024)
            {
                fprintf(stderr, "too many clients.\n");
                close(clientSocket);
            }
            if (i > maxi)
            {
                maxi = i;
            }
            --readyNum;
            if (readyNum == 0)
            {
                continue;
            }
        }

        // 查看其他有活动的客户端套接字
        int sockfd;
        char buf[1024];
        for (int i = 1; i <= maxi; ++i)
        {
            if (clients[i].fd == -1)
            {
                continue;
            }
            if (clients[i].revents & (POLLRDNORM | POLLERR))    // 这个套接字有读事件或错误
            {
                int n;
                sockfd = clients[i].fd;
                n = read(sockfd, buf, 1024);
                if (errno == ECONNRESET)
                {
                    close(sockfd);
                    clients[i].fd = -1;
                }
                else if (n == 0)
                {
                    printf("have client close.\n");
                    close(sockfd);
                    clients[i].fd = -1;
                }
                else if (n < 0)
                {
                    perror("read");
                    exit(-1);
                }
                else
                {
                    printf("[%d]:%s", sockfd, buf);
                    write(sockfd, buf, n);
                }
                --readyNum;
                if(readyNum == 0)
                {
                    break;
                }
            }
        }
    }
}