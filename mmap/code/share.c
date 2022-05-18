/**
 * @file share.c
 * @author your name (you@domain.com)
 * @brief 使用mmap开辟共享内存，实现父子进程之间的通信
 * 这是最优的共享内存
 * @version 0.1
 * @date 2022-02-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#define MEMSIZE 1024
#define _DEFAULT_SOURCE
#define _POSIX_MMAP
int main()
{
    // 这个宏有点奇怪，他有两个条件宏没有定义，导致这个宏没有定义，但是gcc可以编译通过
    char *ptr = mmap(NULL,MEMSIZE,PROT_READ | PROT_WRITE, \
                    MAP_SHARED | MAP_ANONYMOUS,-1,0);
    if(ptr == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }

    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        munmap(ptr,MEMSIZE);
        exit(1);
    }
    if(pid == 0) //子进程
    {
        // write
        strcpy(ptr,"this is child process!");
        munlock(ptr,MEMSIZE);
        exit(0);
    }
    else
    {
        //父进程 read
        wait(NULL);
        printf("this is father process:%s\n",ptr);
        munmap(ptr,MEMSIZE);
        exit(0);
    }

    exit(0);
}