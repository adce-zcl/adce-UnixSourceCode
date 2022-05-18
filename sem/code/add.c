/**
 * @file add.c
 * @author your name (you@domain.com)
 * @brief 信号量数组实现 -- PV操作
 * 忘了写线程，自己加上
 * @version 0.1
 * @date 2022-02-08
 * @copyright Copyright (c) 2022
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#define THRNUM 20
#define LINESIZE 1024
#define FILENAME "/tmp/out"
int semid;
static void P(void)             // 取资源量
{
    struct sembuf op[1];
    op[0].sem_num = 0;          // 要操作的信号量的下标
    op[0].sem_op = -1;          // 操作，是p就-1,是v就+1
    op[0].sem_flg = 0;          // 特殊要求，没有就写0
    while(semop(semid,op,1) < 0)
    {
        if(errno != EINTR || errno != EAGAIN)
        {
            perror("semop()");
            exit(1);
        }
    }
}
static void V(void)             // 还资源量
{
    struct sembuf op[1];
    op[0].sem_num = 0;
    op[0].sem_op = +1;
    op[0].sem_flg = 0;
    if(semop(semid,op,1) < 0)
    {
        perror("semop()");
        exit(1);
    }
}

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
    P();
    fgets(linebuf,LINESIZE,fp);
    // 定位
    fseek(fp,0,SEEK_SET); // 定位到文件首,目的是覆盖写？
    // 加1；
    int num = atoi(linebuf)+1;
    // 写回去
    fprintf(fp,"%d\n",num);
    fflush(fp);
    fclose(fp);
    // 解锁
    V();
}

int main()
{
    // IPC_PRIVATE匿名IPC，因为父子进程有关系
    semid = semget(IPC_PRIVATE,1,0600);
    if(semid < 0)
    {
        perror("semget()");
        exit(1);
    }

    // 这个数组只有一个元素，就是0下标，他的初始信号量为1,就相当于互斥量
    if(semctl(semid,0,SETVAL,1) < 0)
    {
        perror("semctl()");
        exit(1);
    }

    // 销毁当前semid
    semctl(semid,0,IPC_RMID);
    exit(0);
}