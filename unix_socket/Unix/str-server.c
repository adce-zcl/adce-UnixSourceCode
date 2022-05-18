/**
 * @file str-server.c
 * 字节流的域套接字
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#define PATHNAME "/home/adce/code_unixnetwork/unix_socket/Unix/file/unix.str"

void str_echo(int connfd)
{
    while (1)
    {
        char buf[1024];
        read(connfd, buf, 1024);
        printf("client:%s\n", buf);
        write(connfd, "OK", sizeof("OK"));
    }
}

int main(int argc, char **argv)
{
    // 创建监听套接字
    int listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("listen");
        exit(-1);
    }
    
    // 删除路径
    unlink(PATHNAME);
    
    // 初始化服务端地址结构体
    struct sockaddr_un serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_LOCAL;
    strncpy(serveraddr.sun_path, PATHNAME, sizeof(PATHNAME));

    // bind
    if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind");
        exit(-1);
    }

    // listen
    if (listen(listenfd, 200) < 0)
    {
        perror("listen");
        exit(-1);
    }

    // 事物
    while (1)
    {
        int connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0)
        {
            perror("accept");
            exit(-1);
        }
        int childpid = fork();
        if (childpid < 0)
        {
            perror("fork");
            exit(-1);
        }
        if (childpid == 0)
        {
            close(listenfd);
            str_echo(connfd);
        }
        close(connfd);
    }
    return 0;
}