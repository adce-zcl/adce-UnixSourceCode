/**
 * @file bind.c
 * Unix域套接字
 * 创建域套接字，绑定路径名，getsockname输出这个这个绑定的路径
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#define PATHNAME "/home/adce/code_unixnetwork/unix_socket/Unix/file/unix.str"
int main(int argc, char **argv)
{
    // 新建socket
    int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    // 删除路径名，防止路径已经存在
    unlink(PATHNAME);
    // 新建域套接字属性结构体并初始化
    struct sockaddr_un addr;
    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, PATHNAME, sizeof(PATHNAME));
    // 绑定
    if (bind(sockfd, (struct sockaddr*)&addr, SUN_LEN(&addr)) < 0)
    {
        perror("bind");
        exit(-1);
    }
    // 通过getsockname获取sock信息
    struct sockaddr_un addr1;
    socklen_t addr1Len = sizeof(addr1);
    bzero(&addr1, addr1Len);
    if (getsockname(sockfd, (struct sockaddr*)&addr1, &addr1Len) < 0)
    {
        perror("getsockname");
        exit(-1);
    }
    printf("bound name = %s, returned len = %d\n", addr1.sun_path, addr1Len);
    return 0;
}