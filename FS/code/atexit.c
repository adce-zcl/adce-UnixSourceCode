/**
 * @file atexit.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-06
 * 
 * @copyright Copyright (c) 2022
 * 测试钩子函数atexit();
 */
void f1(void);
void f2(void);
#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("begin\n");
    atexit(f1);
    atexit(f2);
    puts("end");
    exit(0);
}
void f1(void)
{
    puts("f1()");
    
}
void f2(void)
{
    puts("f2()");
}