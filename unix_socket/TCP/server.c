/**
 * @file server.c
 * 残缺版本
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#define PORT 1227
#define ADDRESS "127.0.0.1"

/**
 * @brief 信号处理函数
 * @param p——无用
 */
void hander(int p)
{
    exit(0);
}

/**
 * @brief 与客户端的交互函数
 * @param connect_socket 
 * @param client_address 
 */
void func(int connect_socket)
{
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    // 得到套接字对端的地址结构
    getpeername(connect_socket, (struct sockaddr*)&client_address, &client_address_len);

    char address[60];
    inet_ntop(AF_INET, &client_address.sin_addr.s_addr, address, 60);
    int port = ntohs(client_address.sin_port);
    printf("[%d]:connect success(%s : %d).\n", getpid(), address, port);

    // 收发数据 
    while (1)
    {
        char buf[1024];
        int n;
        while (read(connect_socket, buf, 1024) > 0)
        {
        }
        printf("[%d]:(%s):%s\n", getpid(), address, buf);
        write(connect_socket, "OK.", 4);
    }
}

int main()
{
    // 信号处理函数
    signal(SIGINT, hander);

    // 新建套接字
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0)
    {
        perror("socket");
        exit(-1);
    }

    // 创建服务端地址结构
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // 绑定地址结构
    if (bind(listen_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("bind");
        exit(-1);
    }

    // 监听
    if(listen(listen_socket, 200) < 0)
    {
        perror("listen");
        exit(-1);
    }
    while (1)
    {
        
        // 接受连接
        int connect_socket;
        connect_socket = accept(listen_socket, NULL, NULL);

        // 创建子进程
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(-1);
        }
        if (pid > 0)    // 父进程继续监听
        {
            close(connect_socket);
            continue;
        }
        // 子进程业务
        close(listen_socket);
        func(connect_socket);
    }
    exit(0);
}