/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022
 * 
 * 信号令牌桶的封装成库
 * 一个token就传送1个字节
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "mytbf.h"
// CPS 每秒钟读取的字符数量
#define CPS 10
#define BUFSIZE 1024
// 令牌最多数量
#define BURST 100

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Use...\n");
        exit(1);
    }

    int sfd,dfd = 1;
    char buf[BUFSIZE];
    int len,ret,pos;

    mytbf_t *tbf; 
    tbf = mytbf_init(CPS,BURST);
    if(tbf == NULL)
    {
        fprintf(stderr,"mytbf_init() failed!\n");
        exit(1);
    }
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

    while (1)
    {
        // 能不能读，就看有没有令牌
        int size = mytbf_fetchtoken(tbf,BUFSIZE);
        if(size < 0)
        {
            fprintf(stderr,"mytbf_fetchtoken():%s\n",strerror(-size));
            exit(1);
        }
        // 实际消耗了len个token
        while((len = read(sfd,buf,size)) < 0)
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
        if(size - len > 0)
        {
            // 归还剩下的token
            mytbf_returntoken(tbf,size-len);
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
    mytbf_destory(tbf);
    exit(0);
}