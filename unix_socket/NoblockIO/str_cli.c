/**
 * @file str_cli.c
 * 非阻塞版本
 * 不建议在网络通信中使用标准IO，虽然有缓冲区
 * 但是标准IO的缓冲区操纵难度大，弊大于利
 * 这里我们使用自己建立的缓冲区数组
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#define BUFSIZE 1024

// 获取时间函数
char* gf_time()
{
    struct timeval tv;
    static char str[30];
    char *ptr;
    if (gettimeofday(&tv, NULL) < 0)
    {
        perror("gettimeofday");
        exit(-1);
    }
    ptr = ctime(&tv.tv_sec);
    strcpy(str, &ptr[11]); // 这是什么说法？这个11？
    // 这是和tcpdump一样的时间格式
    // 10:18:34.544636 .后面是微妙？
    snprintf(str + 8, sizeof(str) - 8, ".%06ld", tv.tv_usec);
    return str;
}

void str_cli(FILE *fp, int sockfd)
{
    char intosoBuf[BUFSIZE];        // 发送缓冲区，标准输入->sendbuf->socketfd
    char sotooutBuf[BUFSIZE];       // 接收缓冲区，socketfd->recvbuf->标准输出
    char *intosoPtrStart, *intosoPtrEnd;    // 介于start和end之间的是将要发送的数据
    char *sotooutPtrStart, *sotooutPtrEnd;
    int stdineof;   // 表示是否读到eof字符（这个只对文件有效）

    // 初始化四个指针
    intosoPtrStart = intosoPtrEnd = intosoBuf;  // 都指向buf头部
    sotooutPtrStart = sotooutPtrEnd = sotooutBuf;
    stdineof = 0;   // 未读到eof

    // 将sockfd，stdin，stdout设置为非阻塞
    int ops = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, ops | O_NONBLOCK);
    ops = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, ops | O_NONBLOCK);
    ops = fcntl(STDOUT_FILENO, F_GETFL, 0);
    fcntl(STDOUT_FILENO, F_SETFL, ops | O_NONBLOCK);

    // 求出最大的文件描述符
    int maxfd = max(sockfd, max(STDIN_FILENO, STDOUT_FILENO));

    // 设置文件描述符集，使用select
    fd_set rset, wset;  // 读，写
    while (1)
    {
        // 文件描述符集合初始化
        FD_ZERO(&rset);
        FD_ZERO(&wset);

        // 根据两个buf的状态，决定监听哪些文件描述符
        // 如果没有读到eof，且尾指针还没有到达缓冲区尾部，说明可以继续从stdin接收数据
        if (stdineof == 0 && intosoPtrEnd < &intosoBuf[BUFSIZE])
        {
            FD_SET(STDIN_FILENO, &rset);    // 从标准输入读
        }
        // 如果尾指针还没有到达sotooutBuf的尾部，说明还可以继续从sockfd接收数据
        if (sotooutPtrEnd < &sotooutBuf[BUFSIZE])
        {
            FD_SET(sockfd, &rset);  // 从套接字读
        }
        // 如果intosoBuf的头尾指针不相等，说明中间有可以发送到sockfd的数据
        if (intosoPtrStart != intosoPtrEnd)
        {
            FD_SET(sockfd, &wset);  // 往套接字里写
        }
        // 如果sotooutBuf的头尾指针不相等，说明中间有数据可以继续往标准输出输出
        if (sotooutPtrStart != sotooutPtrEnd)
        {
            FD_SET(STDOUT_FILENO, &wset);   // 可以往标准输出写
        }

        // 调用select
        select(maxfd + 1, &rset, &wset, NULL, NULL);    // 这一步被阻塞

        // 解除阻塞，依次判断

        // 如果标准输入有数据可以读
        if (FD_ISSET(STDIN_FILENO, &rset))
        {
            int n;
            // 从标准输入读，读到end指针之后，只剩下bufsize-end这么大的空间
            if ((n = read(STDIN_FILENO, intosoPtrEnd, &intosoBuf[BUFSIZE] - intosoPtrEnd)) < 0) // 暂时没有数据？
            {
                if (errno != EWOULDBLOCK)   // 如果errno不是假错
                {
                    fprintf(stderr, "read error on stdin.\n");
                    return;
                }
            }
            else if (n == 0)    // 如果客户端断开，就会从stdin读到eof，套接字也会收到eof
            {
                fprintf(stderr, "%s:EOF on stdin.\n", gf_time());
                stdineof = 1;   // 设置标志，已经输入eof
                // 如果发送缓冲区已经没有要发送给套接字的数据了
                if (intosoPtrStart == intosoPtrEnd)
                {
                    // 关闭套接字的写端
                    shutdown(sockfd, SHUT_WR);
                }
            }
            else    // 正常接收到数据
            {
                // 这里往stderr发，不是说出错了，是因为stdin和stdout都被占用
                // 如果再往stdin和stdout发数据，可能会导致紊乱，stderr默认也是绑定到终端的
                fprintf(stderr, "%s:read %d bytes from stdin.\n", gf_time());
                intosoPtrEnd += n;  // 指针后移
                FD_SET(sockfd, &wset);  // buf中有数据，就可以往套接字中发送了
            }
        }
        // 如果套接字可读，有数据从套接字发送过来
        if (FD_ISSET(sockfd, &rset))
        {
            int n;
            if ((n = read(sockfd, sotooutPtrEnd, &sotooutBuf[BUFSIZE] - sotooutPtrEnd)) < 0)
            {
                // 正常来讲这种情况是不会发生的，已经发生事件，却还是没有数据可读
                if (errno != EWOULDBLOCK)
                {
                    fprintf(stderr, "read error on sockfd.\n");
                    return;
                }
            }
            else if (n == 0)    // 读到EOF，说明没有数据要接受了
            {
                fprintf(stderr, "%s:EOF on sockfd.\n", gf_time());
                if (stdineof == 1)  // 如果没有数据要发送了
                {
                    return; // 既没有发送了，也没有接受了，就结束
                }
                // 否则就是服务器宕掉了
                fprintf(stderr, "str_cli:server terminated prematurely.");
                return;
            }
            else
            {
                fprintf(stderr, "%s:read %d bytes from sockfd.\n", gf_time(), n);
                sotooutPtrEnd += n;
                // 既然缓冲区有数据了，就可以往标准输出写了
                FD_SET(STDOUT_FILENO, &wset);
            }
        }
        // 如果标准输出可以写
        int n;
        if (FD_ISSET(STDOUT_FILENO, &wset) && (n = sotooutPtrEnd - sotooutPtrStart) > 0)
        {
            int nwriten;
            if ((nwriten = write(STDOUT_FILENO, sotooutPtrStart, n)) < 0)
            {
                // 正常来讲这种情况是不会发生的，已经发生事件，却还是没有数据可读
                if (errno != EWOULDBLOCK)
                {
                    fprintf(stderr, "write error on stdout.\n");
                    return;
                }
            }
            else
            {
                fprintf(stderr, "%s:write %d bytes to stdout.\n", gf_time(), nwriten);
                sotooutPtrStart += nwriten;
                if (sotooutPtrStart == sotooutPtrEnd)   // 没有要发送的数据了，重置缓冲区
                {
                    sotooutPtrStart = sotooutPtrEnd = sotooutBuf;
                }
            }
        }
        // 如果套接字可写
        int n;
        if (FD_ISSET(sockfd, &wset) && (n = intosoPtrEnd - intosoPtrStart) > 0)
        {
            int nwriten;
            if ((nwriten = write(sockfd, intosoPtrStart, n)) < 0)
            {
                // 正常来讲这种情况是不会发生的，已经发生事件，却还是没有数据可读
                if (errno != EWOULDBLOCK)
                {
                    fprintf(stderr, "write error on sockfd.\n");
                    return;
                }
            }
            else
            {
                fprintf(stderr, "%s:write %d bytes to sockfd.\n", gf_time(), nwriten);
                intosoPtrStart += nwriten;
                if (intosoPtrStart == intosoPtrEnd)   // 没有要发送的数据了，重置缓冲区
                {
                    sotooutPtrStart = sotooutPtrEnd = sotooutBuf;
                    if (stdineof == 1)  // 已经没有数据要发送了
                    {
                        shutdown(sockfd, SHUT_WR);
                    }
                }
            }
        }
    }
}