#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
void int_handler(int a)                     // 信号函数
{
    alarm(2);                               // 再次定时
    printf("Get SIGALRM\n");
}

int main()
{
    signal(SIGALRM,int_handler);            // 注册信号，定时信号
    alarm(5);                               // 定时时间为5秒钟
    while (1)
    {
        printf("while(1)\n");
        sleep(2);
    }
    
    exit(0);
}