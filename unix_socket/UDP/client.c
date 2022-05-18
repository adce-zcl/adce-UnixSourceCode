/**
 * @file client.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void dg_cli(FILE *fp, int sockfd, struct sockaddr *serveraddr, socklen_t addrlen)
{
    char buf[1024];
    while (fgets(buf, 1024, fp) != NULL)
    {
        sendto(sockfd, buf, strlen(buf), 0, serveraddr, addrlen);
        int n = recvfrom(sockfd, buf, 1024, 0, NULL, NULL);
        buf[n] = 0;
        fputs(buf, stdout);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Using: ./program ip port\n");
        exit(-1);
    }
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
    serveraddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr.s_addr);

    dg_cli(stdin, sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    exit(0);
}