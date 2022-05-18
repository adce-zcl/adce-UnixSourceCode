/**
 * @file pipe.c
 * @author your name (you@domain.com)
 * @brief 利用内核提供的管道机制，完成进程之间的通信
 * 一定是先pipe再fork
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
    if(pid == 0)        // child read
    {
        close(pd[1]);   // 关闭写端
        len = read(pd[0],buf,BUFSIZE);
        write(1,buf,len);
        close(pd[0]);
        exit(0);
    }
    else                // parent write
    {
        close(pd[0]);
        write(pd[1],"Hello",sizeof("Hello"));
        close(pd[1]);
        wait(NULL);
        exit(0);
    }
}