/**
 * @file add.c
 * @author your name (you@domain.com)
 * @brief 使用lockf文件锁，由多线程并发，改写成多进程并发
 * @version 0.1
 * @date 2022-02-08
 * 
 * @copyright Copyright (c) 2022
 * 
 * 测试线程竞争
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define PROCNUM 20
#define LINESIZE 1024
#define FILENAME "/tmp/out"
// #define _DEFAULT_SOURCE

static void func_add(void)
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

    int fd = fileno(fp);
    lockf(fd,F_LOCK,0);
    fgets(linebuf,LINESIZE,fp);
    // 定位
    fseek(fp,0,SEEK_SET); // 定位到文件首,目的是覆盖写？
    sleep(1);
    // 加1；
    int num = atoi(linebuf)+1;
    // 写回去
    fprintf(fp,"%d\n",num);
    fflush(fp);
    
    lockf(fd,F_ULOCK,0);
    fclose(fp);
    return ;

}

int main()
{
    int err;
    pid_t pid;
    for (int i = 0; i < PROCNUM; i++)
    {
        // 创建进程
        pid = fork();
        if(pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        if(pid == 0) // 子进程
        {
            func_add();
            exit(0);
        }
    }
    for (int i = 0; i < PROCNUM; i++)
    {
        wait(NULL);
    }

    // 销毁
    exit(0);
}