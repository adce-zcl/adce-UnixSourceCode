/**
 * @file slowcat.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022
 * 
 * 使用alarm来实现cat的流量控制
 * 漏桶的实现
 * 使用setitimer代替alarm
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

// CPS 每秒钟读取的字符数量
#define CPS 10
#define BUFSIZE CPS
static volatile int loop = 0;
static void alrm_handler(int s)
{
    loop = 1;
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

    // alarm(1);
    struct itimerval itv;
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    if(setitimer(ITIMER_REAL, &itv, NULL) < 0)
    {
        perror("setitiemr()");
        exit(1);
    }
    while (1)
    {
        while (!loop)
        {
            // 不让忙等,让程序暂停，直到信号到来
            pause(); 
        }
        loop  = 0;
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