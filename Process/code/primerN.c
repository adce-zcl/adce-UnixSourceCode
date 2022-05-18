/**
 * @file primerN.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-10
 * 
 * @copyright Copyright (c) 2022
 * 
 * 使用交叉分配法调度多进程
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LEFT 30000000
#define RIGHT 30000200
// 进程的数量
#define N 3
int main()
{
    int mark = 1;
    int count = 0;
    pid_t pid;

    for(int n = 0; n < N; n++)
    {
        pid = fork();
        if(pid < 0)
        {
            perror("fork()");
            // 回收子进程资源
            exit(1);
        }
        if(pid == 0)
        {
            for(int i = LEFT+n; i < RIGHT; i+=N)
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
                    printf("[%d] %d is primer.\n",n,i);
                }
                
            }
            exit(0);
        }
    }

    for(int n = 0; n < N; n++)
    {
        wait(NULL);
    }
    exit(0);
}