/**
 * @file share.c
 * @author your name (you@domain.com)
 * @brief 使用共享内存实现
 * @version 0.1
 * @date 2022-02-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#define MEMSIZE 1024
int main()
{
    int shmid;
    pid_t pid;
    char *ptr;
    shmid = shmget(IPC_PRIVATE,MEMSIZE,0600);       
    if(shmid < 0)
    {
        perror("shmget()");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0)       // child write
    {
        ptr = shmat(shmid,NULL,0);
        if(ptr == (void*)-1)
        {
            perror("shmat()");
            exit(1);
        }
        strcpy(ptr,"Hello shmat write!");
        shmdt(ptr);         // 解除映射
        exit(0);
    }
    else            // parent read
    {
        wait(NULL);
        ptr = shmat(shmid,NULL,0);
        if(ptr == (void*)-1)
        {
            perror("shmat()");
            exit(1);
        }
        puts(ptr);
        shmdt(ptr);
        shmctl(shmid,IPC_PRIVATE,NULL);
        exit(0);
    }
}