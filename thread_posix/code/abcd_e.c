/**
 * @file abcd_e.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 * 使用条件变量实现
 * a打印完了通知b
 * b打印完了通知c
 * ...
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#define THRNUM 4
static int num = 0;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int next(int n)
{
    if(n == 3)
    {
        return 0;
    }
    return n+1;
}

static void* thr_func(void *p)
{
    int n = (int)p;
    char c = 'a' + (int)p;
    // 使用信号杀死
    
    while (1)
    {
        pthread_mutex_lock(&mut);
        while (num != n)
        {
            pthread_cond_wait(&cond,&mut);
        }
        
        write(1,&c,1);
        num = next(num);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mut);
    }

    pthread_exit(NULL);
}

int main()
{
    int err = 0;
    pthread_t tid[THRNUM];
    for (int i = 0; i < THRNUM; i++)
    {
        err = pthread_create(tid+i,NULL,thr_func,(void*)i);
        if(err)
        {
            fprintf(stderr,"pthread_create():%s\n",strerror(err));
            exit(1);
        }
    }

    alarm(5);

    for (int i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[i],NULL);
    }
    
    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cond);

    exit(0);
}
