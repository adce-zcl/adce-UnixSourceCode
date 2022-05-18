/**
 * @file 5sec_sig.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-11
 * 
 * @copyright Copyright (c) 2022
 * 
 * 使用信号计时
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
static volatile int loop = 1;
// volatile关键字表明这是一个经常变化的量
// 它告诉编译器，要去真正存放loop的地方去取，而不要相信内存
// 这个关键字在嵌入式编程中很常用，因为可以防止优化带来的错误

static void alrm_handler(int s)
{
    loop = 0;
}
int main()
{
    long long int count = 0;
    alarm(5);
    signal(SIGALRM,alrm_handler);
    while (loop)
    {
        count++;
    }
    printf("%lld\n",count);
    exit(0);
}