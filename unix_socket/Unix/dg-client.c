#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#define PATHNAME "/home/adce/code_unixnetwork/unix_socket/Unix/file/unix.dg"
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
    int sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("sockfd");
        exit(-1);
    }
    
    // 域套接字与udp不同的地方是，客户端也需要bind
    struct sockaddr_un clientaddr;
    bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sun_family = AF_LOCAL;

    if (bind(sockfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) < 0)
    {
        perror("bind");
        exit(-1);
    }

    // 服务端的地址信息结构
    struct sockaddr_un serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_LOCAL;
    strncpy(serveraddr.sun_path, PATHNAME, sizeof(PATHNAME));

    dg_cli(stdin, sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    return 0;
}
