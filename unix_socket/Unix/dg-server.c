/**
 * @file dg-server.c
 * 感觉数据报的没什么用，也不会用udp传输描述符
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#define PATHNAME "/home/adce/code_unixnetwork/unix_socket/Unix/file/unix.dg"
void dg_echo(int sockfd)
{
    struct sockaddr_un childaddr;
    socklen_t childaddrlen;
    char buf[1024];
    int n;
    while (1)
    {
        bzero(buf, 0);
        n = recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *)&childaddr, &childaddrlen);
        printf("msg:%s", buf);
        sendto(sockfd, buf, n, 0, (struct sockaddr *)&childaddr, childaddrlen);
    }
}
int main(int argc, char **argv)
{
    int sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(-1);
    }

    unlink(PATHNAME);

    struct sockaddr_un serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_LOCAL;
    strncpy(serveraddr.sun_path, PATHNAME, sizeof(PATHNAME));

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind");
        exit(-1);
    }

    // 事物
    dg_echo(sockfd);
}