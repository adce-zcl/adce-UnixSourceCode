/**
 * @file timelog.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-06
 * 
 * @copyright Copyright (c) 2022
 * 程序要求在文件中保存时间戳，要求对文件续写，文件不存在
 * 则创建文件
 */
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define FNAME "./../file/time.txt"
#define BUFSIZE 1024
int main()
{
    FILE *fp;
    char buf[BUFSIZ];
    int count = 0;
    time_t stamp;
    struct tm *tm;

    fp = fopen(FNAME,"a+");
    if(fp == NULL)
    {
        perror("fopen()\n");
        exit(1);
    }
    while(fgets(buf,BUFSIZ,fp) != NULL)
    {
        count++;
    }
    // 这是一个死循环，正常情况，类似这样的程序
    // 我们都是使用进程之间通信来杀死他，或者使用
    // 钩子函数
    while(1)
    {
        time(&stamp);
        tm = localtime(&stamp);
        // 左对齐
        fprintf(fp,"%-4d%d-%d-%d %d:%d:%d\n",\
        ++count,\
        tm->tm_year+1900,\
        tm->tm_mon+1,\
        tm->tm_mday,\
        tm->tm_hour,\
        tm->tm_min,\
        tm->tm_sec);
        fflush(fp);
        sleep(1);
    }

    fclose(fp);
    exit(0);
}