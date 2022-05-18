/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-15
 * 
 * @copyright Copyright (c) 2022
 * 
 * 任务结构体应该包含的内容：
 * 1.几秒之后执行的秒数
 * 2.执行任务的函数
 * 3.函数的参数
 * 
 * 工作思路：
 * 每过一秒钟，把任务列表的所有任务的秒数-1
 * 当出现0秒的时候，执行对应的任务，清空该任务即可
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "anytime.h"

static void f1(void *p)
{
    printf("f1():%s\n",p);
}

static void f2(void *p)
{
    printf("f2():%s\n",p);
}

int main()
{
    puts("Begin!");
    // 5秒钟打印f1
    int job1 = at_addjob(5,f1,"aaa");
    if(job1 < 0)
    {
        fprintf(stderr,"job1:%s\n",strerror(-job1));
        exit(1);
    }
    // 2秒钟打印f2
    int job2 = at_addjob(2,f2,"bbb");
    if(job2 < 0)
    {
        fprintf(stderr,"job2:%s\n",strerror(-job2));
        exit(1);
    }
    // 7秒钟打印f1
    int job3 = at_addjob(7,f1,"ccc");
    if(job3 < 0)
    {
        fprintf(stderr,"job3:%s\n",strerror(-job3));
        exit(1);
    }
    puts("End!");
    while (1)
    {
        write(1,".",1);
        sleep(1); // 暂时使用sleep
    }
    
    exit(0);
}