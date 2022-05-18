/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 中继引擎
 * @version 0.1
 * @date 2022-02-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "relayer.h"
// 两个设备
#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"
#define TTY3 "/dev/tty10"
#define TTY4 "/dev/tty4"

int main()
{
    
    int fd1 = open(TTY1,O_RDWR);
    if(fd1 < 0)
    {
        perror("open()");
        exit(1);
    }
    write(fd1,"tty1\n",5);
    int fd2 = open(TTY2,O_RDWR | O_NONBLOCK);
    if(fd2 < 0)
    {
        perror("open()");
        exit(1);
    }
    write(fd2,"tty2\n",5);

    int job1 = rel_addjob(fd1,fd2); //将工作添加的工作数组，返回状态
    if(job1 < 0) // 如果失败
    {
        fprintf(stderr,"rel_addjob():%s\n",strerror(-job1));
        exit(1);
    }

    int fd3 = open(TTY3,O_RDWR);
    if(fd1 < 0)
    {
        perror("open()");
        exit(1);
    }
    write(fd3,"tty3\n",5);
    int fd4 = open(TTY4,O_RDWR);
    if(fd2 < 0)
    {
        perror("open()");
        exit(1);
    }
    write(fd4,"tty4\n",5);

    int job2 = rel_addjob(fd3,fd4); //将工作添加的工作数组，返回状态
    if(job2 < 0) // 如果失败
    {
        fprintf(stderr,"rel_addjob():%s\n",strerror(-job2));
        exit(1);
    }

    while(1)
    {
        pause;
    }

    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);
    exit(0);
}
