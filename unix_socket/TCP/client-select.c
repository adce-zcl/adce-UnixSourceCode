/**
 * @file client-select.c
 * 使用select改进client1.c中的str_cli函数
 * 这个str_cli函数是不正确的：
 * 1.标准IO缓冲区不会触发select，不建议待缓冲区的IO和select一起使用
 * 2.写完就关闭套接字，可能会有数据遗留在网络中，应该先进行半关闭
 * 
 */
// 只负责连接
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
void hander(int p)
{
    exit(0);
}

void str_cli(FILE *fp, int client_socket)
{
    fd_set readFds;     // 用于select监听的文件描述符集合
    FD_ZERO(&readFds);  // 初始化为0
    int maxfd = fileno(fp) > client_socket ? fileno(fp) : client_socket + 1; // 最大的文件描述符 + 1
    while (1)
    {
        printf("...\n");
        FD_SET(fileno(fp), &readFds);       // 监听fp文件描述符——每次循环都必须重置
        FD_SET(client_socket, &readFds);    // 监听套接字
        select(maxfd, &readFds, NULL, NULL, NULL);      // 监听文件描述符集合里的 读 事件 —— 这一步被阻塞

        /* 解除阻塞执行下面 */

        if(FD_ISSET(client_socket, &readFds))   // 如果client_socket的读事件触发
        {
            // 开始从client_socket中读数据，显示到终端
            printf("read msg.\n");
            char buf[1024];
            int n;
            if ((n = read(client_socket, buf, 1024)) > 0)
            {
                write(STDOUT_FILENO, buf, n);
            }
            if (n == -1)
            {
                perror("read");
            }
        }
        if (FD_ISSET(fileno(fp), &readFds))     // 标准输入事件触发
        {
            printf("stdin write.\n");
            char buf[1024];
            int n;
            n = read(STDIN_FILENO, buf, 1024);
            if (n == -1)
            {
                perror("read");
                exit(-1);
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