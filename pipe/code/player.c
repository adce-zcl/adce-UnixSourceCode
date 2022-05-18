/**
 * @file pipe.c
 * @author your name (you@domain.com)
 * @brief 利用内核提供的管道机制，完成进程之间的通信
 * 一定是先pipe再fork
 * 利用管道和mpg123播放音频
 * 匿名管道
 * @version 0.1
 * @date 2022-02-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUFSIZE 1024
int main()
{
    int pd[2];
    if(pipe(pd) < 0)
    {
        perror("pipe()");
        exit(1);
    }
    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    char buf[BUFSIZE];
    int len = 0;
    int fd;
    if(pid == 0)        // child read
    {
        close(pd[1]);
        dup2(pd[0],0);      // 将pd[0]重定向到0号
        close(pd[0]);
        fd = open("/dev/null",O_RDWR);
        dup2(fd,1);
        dup2(fd,2);
        execl("/usr/bin/mpg123","mpg123","-",NULL);     // 当前子进程编程mpg123程序
        perror("execl()");      // 正常来讲执行不到这句，因为在上面就变成其他进程了
        exit(0);
    }
    else                // parent write
    {
        close(pd[0]);       // 关闭读端

        /*
        // 父进程从网上收数据，往管道中写
        */

        close(pd[1]);
        wait(NULL);
        exit(0);
    }
}