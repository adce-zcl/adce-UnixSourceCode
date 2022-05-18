/**
 * @file create2.c
 * @author your name (you@domain.com)
 * @brief 测试一个进程可以创建多少个线程
 * @version 0.1
 * @date 2022-02-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

static void* func(void *p);
int main()
{
    pthread_t tid;
    int i;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr,1024*1024); //设置栈的大小
    for (i = 0; ; i++)
    {
        int err = pthread_create(&tid,&attr,func,NULL);
        if(err){
            fprintf(stderr,"pthread_create():%s\n",strerror(err));
            break;
        }
    }
    printf("%d\n",i);
    pthread_attr_destroy(&attr);
    exit(0);
}
static void* func(void *p){
    while (1)
    {
        
    }
    
}