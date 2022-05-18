/**
 * @file client-select.c
 * 使用select改进client1.cpp中的str_cli函数
 * 正确版本
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <sys/select.h>
void hander(int p)
{
    exit(0);
}

void str_cli(FILE *fp, int client_socket)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    int maxfd = fileno(fp) > client_socket ? fileno(fp) : client_socket + 1;
    int stdineof = 0;   // 标志 是否发生了标准输入事件
    while (1)
    {
        if (stdineof == 0)
        {
            FD_SET(fileno(fp), &readfds);
        }
        FD_SET(client_socket, &readfds);
        select(maxfd, &readfds, NULL, NULL, NULL);
        if (FD_ISSET(client_socket, &readfds))
        {
            int n;
            char buf[1024];
            if ((n = read(client_socket, buf, 1024)) == 0)
            {
                if (stdineof == 1)  // 表示输入完毕了，可以直接退出
                {
                    return;
                }
            }
            write(fileno(fp), buf, n);
        }
        if (FD_ISSET(fileno(fp), &readfds))
        {
            int n;
            char buf[1024];
            n = read(fileno(fp), buf, 1024);
            if (n == 0)     // 没有输入，则表示输入完毕
            {
                stdineof = 1;
                shutdown(client_socket, SHUT_WR);   // 关闭套接字的写端
                FD_CLR(fileno(fp), &readfds);   // 不再监听fp
            }
            write(client_socket, buf, n);
        }
    }

    return;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr,"Using: ./filename address port\n");
        exit(0);
    }
    signal(SIGINT, hander);

    int port = atoi(argv[2]);
    
    // 新建套接字
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("socket");
        exit(-1);
    }

    // 建立服务端地址结构体
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &server_address.sin_addr.s_addr);

    // 请求连接
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("connect");
        close(client_socket);
        exit(-1);
    }
    printf("connect success, IP:%s, port:%d\n", argv[1], port);
    str_cli(stdin, client_socket);

    exit(0);
}