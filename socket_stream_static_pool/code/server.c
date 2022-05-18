#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "proto.h"
#define IPSTRSIZE 40
#define BUFSIZE 1024
#define PROCNUM 4 // 进程上线
static void server_job(int serversocket)
{
    char buf[BUFSIZE];
    time_t t = time(NULL);
    int len = sprintf(buf, FMT_STAMP, t);    // 把t写到buf中
    int s = send(serversocket, buf, len, 0); // 把buf的内容通过socket发送
    if (s < 0)
    {
        perror("send()");
        exit(1);
    }
}

static void server_loop(int listen_socket)
{
    // 接收连接
    struct sockaddr_in clientaddr;
    memset(&clientaddr, 0, sizeof(clientaddr));
    socklen_t clientaddrlen = sizeof(clientaddr);
    char ipstr[IPSTRSIZE];
    while (1)
    {
        // 此时的a是用来通信的socket
        // accept本身是互斥的
        int a = accept(listen_socket, (void *)&clientaddr, &clientaddrlen);
        if (a < 0)
        {
            perror("accept()");
            exit(1);
        }

        inet_ntop(AF_INET, &clientaddr.sin_addr, ipstr, IPSTRSIZE);
        int port = ntohs(clientaddr.sin_port);
        printf("[%d]client:%s:%d\n",getpid(), ipstr, port);

        // 收发数据
        server_job(a);
        close(a);
    }
}

int main()
{
    // 新建socket
    int listen_socket;
    listen_socket = socket(AF_INET, SOCK_STREAM, 0 /*IPPROTO_TCP*/);
    if (listen_socket < 0)
    {
        perror("socket()");
        exit(1);
    }

    // 设置socket属性,使其能重复使用,原则是收回不用的端口号,重新使用
    // 可以防止bind() 错误,地址已经在使用
    int val = 1;
    int set = setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (set < 0)
    {
        perror("setsocketopt()");
        exit(1);
    }

    // 给socket建立地址
    struct sockaddr_in localaddr;
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, "127.0.0.1", &localaddr.sin_addr);
    int b = bind(listen_socket, (void *)&localaddr, sizeof(localaddr));
    if (b < 0)
    {
        perror("bind()");
        exit(1);
    }

    // 将socket设置为监听
    int l = listen(listen_socket, 200);
    if (l < 0)
    {
        perror("listen()");
        exit(1);
    }

    // 产生并发
    pid_t pid;
    for (int i = 0; i < PROCNUM; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        if (pid == 0)
        {
            server_loop(listen_socket);
            exit(0);
        }
    }

    for (int i = 0; i < PROCNUM; i++)
    {
        wait(NULL);
    }
    close(listen_socket);
    exit(0);
}
