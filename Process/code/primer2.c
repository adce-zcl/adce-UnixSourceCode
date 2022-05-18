/**
 * @file primer2.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-10
 * 
 * @copyright Copyright (c) 2022
 * 进程的个数是有限的
 * 多进程分配算法：分块算法，交叉分配，池类算法
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LEFT 30000000
#define RIGHT 30000200
int main()
{
    int mark = 1;
    int count = 0;
    pid_t pid;
    for(int i = LEFT; i < RIGHT; i++)
    {
        pid = fork();
        if(pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        if(pid == 0)
        {
            mark = 1;
            for(int j = 2; j < i/2; j++)
            {
                if(i%j == 0)
                {
                    mark = 0;
                    break;
                }
            }
            if(mark)
            {
                count++;
                printf("%d is primer.\n",i);
            }
            
            exit(0);
        }
    }

    for(int i = LEFT; i <= RIGHT; i++)
    {
        wait(NULL);
    }

    exit(0);
}