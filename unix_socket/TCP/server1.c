/**
 * @file server1.cpp
 * 除IO外，正规版本
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>
#include <sys/wait.h>
#include <errno.h>

#define PORT 1226
#define ADDRESS "127.0.0.1"

/**
 * @brief 信号处理函数
 * 负责给断开连接的客户端的子进程收尸
 */
void hander(int p)
{
    pid_t pid;
    int stat;
    // 必须要waitpid，-1表示可以回收任何进程
    // WNOHANG表示如果没有需要回收的进程，不阻塞
    // 如果调用wait，只会回收一个，而信号默认是不排队的；
    // 如果同一时刻大量信号涌来，信号处理函数就只会被执行一次，导致回收不干净
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        printf("child %d terminated.\n", pid);
    }
    // 正常规范要return
    return;
}

/**
 * @brief 与客户端的交互函数
 * @param connect_socket 
 * @param client_address 
 */
void str_echo(int connect_socket)
{
    ssize_t n;
    char buf[1024];
    while (1)
    {
        while ((n = read(connect_socket, buf, 1024)) > 0)
        {
            printf("msg:%s\n", buf);
            write(connect_socket, buf, n);
        }
        if (n < 0)
        {
            perror("read");
            break;
        }
    }
}

int main()
{
    // 信号处理函数，使用这个也可以
    signal(SIGCHLD, hander);

    // 新建套接字
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0)
    {
        perror("socket");
        exit(-1);
    }

    // 创建服务端地址结构
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address)); // 初始化，这句加不加的，加上更好
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDRESS, &server_address.sin_addr.s_addr);

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
        int connect_socket;
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);

        // 接受连接
        connect_socket = accept(listen_socket, (struct sockaddr*)&client_address, &client_address_len);
        // 这是因为慢系统调用被信号打断，假错为EINTR，要单独判断，不是所有系统都会重启系统调用；
        // 基本所有的慢系统调用都有被信号误打断的可能， 所以要记住这一点；
        // connect不能重启，重启会直接报错，必须重新建立socket
        if (errno == EINTR) 
        {
            continue;
        }
        // 创建子进程
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(-1);
        }
        if (pid == 0)    // 子进程
        {
            close(listen_socket);
            str_echo(connect_socket);
            exit(0);
        }
        // 父进程继续从就绪队列取出连接
        close(connect_socket);
    }
    exit(0);
}