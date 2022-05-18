/**
 * @file slowcat2.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022
 * 
 * 令牌桶实现
 * 令牌：token
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

// CPS 每秒钟读取的字符数量
#define CPS 10
#define BUFSIZE CPS
// 令牌最多数量
#define BURST 100
static volatile sig_atomic_t token = 0; //令牌 原子操作
static void alrm_handler(int s)
{
    alarm(1);
    token++;
    if(token > BURST)
    {
        token = BURST;
    }
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Use...\n");
        exit(1);
    }

    signal(SIGALRM,alrm_handler);
    int sfd,dfd = 1;
    char buf[BUFSIZE];
    int len,ret,pos;
    
    do
    {
        sfd = open(argv[1],O_RDONLY);
        if(sfd < 0)
        {
            if(errno != EINTR)
            {
                perror("open()");
                exit(1);
            }
        }
    } while (sfd < 0);

    alarm(1);
    while (1)
    {
        while (token <= 0)
        {
            // 不让忙等,让程序暂停，直到信号到来
            pause(); 
        }
        token--;
        while((len = read(sfd,buf,BUFSIZE)) < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            perror("read()");
            break;
        }
        if(len == 0)
        {
            break;
        }
        pos = 0;
        while(len > 0)
        {
            ret = write(dfd,buf+pos,len);
            if(ret < 0)
            {
                if(errno == EINTR)
                {
                    continue;
                }
                perror("write()");
                exit(1);
            }
            pos += ret; 
            len -= ret;
        }
    }
    close(sfd);
    exit(0);
}