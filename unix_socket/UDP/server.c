/**
 * @file server.c
 * UDP回射程序
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define ADDRESS "127.0.0.1"
#define PORT 1227

void dg_echo(int sockfd)
{
    struct sockaddr_in childaddr;
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

int main(int agrc, char **argv)
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in serveraddr;
    bzero(&serveraddr, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDRESS, &serveraddr.sin_addr.s_addr);

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind");
        close(sockfd);
        exit(-1);
    }

    // 执行事物
    dg_echo(sockfd);
}