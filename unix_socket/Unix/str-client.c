/**
 * @file str-client.c
 * 流式域套接字客户端
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
        fgets(buf, 1024, stdin);
        write(connfd, buf, strlen(buf));
        bzero(buf, 1024);
        read(connfd, buf, 1024);
        printf("recv:%s\n", buf);
    }
}
int main(int argc, char **argv)
{
    // 新建套接字
    int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(-1);
    }

    // 建立服务端的地址信息结构体
    struct sockaddr_un serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_LOCAL;
    strncpy(serveraddr.sun_path, PATHNAME, sizeof(PATHNAME));

    // 建立连接
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("connect");
        exit(-1);
    }

    // 执行事物
    str_echo(sockfd);
    return 0;
}