/**
 * @file 100day.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-06
 * 
 * @copyright Copyright (c) 2022
 * 这个程序计算，从今天开始的100天以后是哪一天，
 * 星期几？
 * 最好的计算方式是tm_mday+=100;
 * 再使用mktime，mktime是把struct tm转换为time_t,这里是利用了
 * mktime的副作用，因为mktime的参数不是const的，他会自动检查时间，
 * 溢出自动进位，这是一个非常nice的副作用
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TIMESTRSIZE 1024
int main()
{
    time_t stamp;
    struct tm *tm;
    char timestr[TIMESTRSIZE];
    stamp = time(NULL);
    tm = localtime(&stamp);
    strftime(timestr,TIMESTRSIZE,"Now:%Y-%m-%d",tm);
    // 先把今天的时间戳打印出来
    puts(timestr);
    tm->tm_mday+=100;// 这个结果肯定是溢出的，因为不会大于31
    // 利用mktime的副作用，自动修正struct tm结构体
    mktime(tm);
    strftime(timestr,TIMESTRSIZE,"100day:%Y-%m-%d",tm);
    puts(timestr);
    exit(0);
}