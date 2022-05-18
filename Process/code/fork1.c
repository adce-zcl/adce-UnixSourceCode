/**
 * @file fork1.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-07
 * 
 * @copyright Copyright (c) 2022
 * 
 * 在终端，\n只是刷新到终端，在重定向文件的时候
 * 因为每一个进程的缓冲区都已经填充了Begin，所以一定要在fork之前刷新缓冲区
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid;
    printf("[%d]:Begin!\n",getpid());
    // 正常应该是Begin只打印一次，但是在重定向到文件的时候，
    // Begin会打印两次，这不是\n的问题
    // 这是因为要刷新所有该刷新的流，且是在fork之前
    fflush(NULL);

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    else if(pid == 0)
    {
        // 这是一个子进程
        
        printf("[%d]:child!\n",getpid());

    }
    else
    {
        // 这是一个父进程
        sleep(1); //不建议用sleep，因为不好移植
        printf("[%d]:father!\n",getpid());
    }
    printf("[%d]:End.\n",getpid());

    // getchar();
    exit(0);
}