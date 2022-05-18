/**
 * @file primer0_pool.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-09
 * 
 * @copyright Copyright (c) 2022
 * 
 * 这是一个忙等的版本，所以cpu占用率比较高
 * 因为要不停的查看状态
 * 最好的是通知法，即信号槽或者监听者模式
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define LEFT 30000000
#define RIGHT 30000200
#define THRNUM 4
static int num = 0; // 表示任务
static pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER; // 互斥量

static void* thr_prime(void *p);
int main()
{

    // 初始化线程环节
    int err = 0;
    pthread_t tid[THRNUM];
    for(int i = 0; i < THRNUM; i++)
    {
        err = pthread_create(tid+i,NULL,thr_prime,(void*)i);
        if(err)
        {
            fprintf(stderr,"pthread_create():%s\n",strerror(err));
            exit(1);
        }
    }

    // 下发任务的环节
    for (int i = LEFT; i <= RIGHT; i++)
    {
        pthread_mutex_lock(&mut_num);
        while (num != 0)
        {
            pthread_mutex_unlock(&mut_num);
            // 这里最好是延迟一下，但是不推荐使用sleep
            //这是针对调度器的函数，所有sched都是针对调度器的函数
            // 功能是让出调度器，进行很少时间的等待，可以理解为很小时间的sleep
            // 并且不会引起调度器的紊乱
            sched_yield(); 
            pthread_mutex_lock(&mut_num);
        }
        num = i;
        pthread_mutex_unlock(&mut_num);
    }
    // 任务结束
    pthread_mutex_lock(&mut_num);
    // 确保最后一个任务被取走
    while(num != 0)
    {
        pthread_mutex_unlock(&mut_num);
        sched_yield();
        pthread_mutex_lock(&mut_num);
    }
    num = -1;
    pthread_mutex_unlock(&mut_num);

    // 收尸环节
    for (int i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[i],NULL);
    }
    
    pthread_mutex_destroy(&mut_num);
    exit(0);
}

static void* thr_prime(void *p)
{
    int i,j,mark;

    while (1)
    {
        pthread_mutex_lock(&mut_num);
        while (num == 0)
        {
            pthread_mutex_unlock(&mut_num);
            sched_yield();
            pthread_mutex_lock(&mut_num);
        }
        if (num == -1)
        {
            pthread_mutex_unlock(&mut_num);
            break;
        }
        i = num;
        num = 0;
        pthread_mutex_unlock(&mut_num);
        
        mark = 1;
        for (j = 2; j < i/2; j++)
        {
            if(i%j == 0)
            {
                mark = 0;
                break;
            }
        }
        if(mark)
        {
            printf("[%d]%d is a primer.\n",(int)p,i);
        }
    }
    pthread_exit(NULL);
}