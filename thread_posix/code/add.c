/**
 * @file add.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-08
 * 
 * @copyright Copyright (c) 2022
 * 
 * 测试线程竞争
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define THRNUM 20
#define LINESIZE 1024
#define FILENAME "/tmp/out"
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER; //初始化

static void* thr_add(void *p)
{
    // 打开
    FILE *fp = fopen(FILENAME,"r+");
    if(fp == NULL)
    {
        perror("fopen()\n");
        exit(1);
    }

    // 读数
    char linebuf[LINESIZE];

    // 加锁
    pthread_mutex_lock(&mut);

    fgets(linebuf,LINESIZE,fp);

    // 定位
    fseek(fp,0,SEEK_SET); // 定位到文件首,目的是覆盖写？
    
    // 加1；
    int num = atoi(linebuf)+1;

    // 写回去
    fprintf(fp,"%d\n",num);
    //fflush(fp);

    fclose(fp);

    // 解锁
    pthread_mutex_unlock(&mut);
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid[THRNUM];
    int err;

    for (int i = 0; i < THRNUM; i++)
    {
        err = pthread_create(tid+i,NULL,thr_add,NULL);
        if(err)
        {
            fprintf(stderr,"pthread_create() error:%s\n",strerror(err));
            exit(1);
        }
    }
    for (int i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[i],NULL);
    }

    // 销毁
    pthread_mutex_destroy(&mut);
    exit(0);
}