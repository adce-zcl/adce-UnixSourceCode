/**
 * @file susp.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-15
 * 
 * @copyright Copyright (c) 2022
 * 信号驱动程序，不能使用pause完成，因为它和其他配合的函数不是原子操作
 * 要使用gissuspend函数
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
static void int_handler(int s)
{
    write(1,"!",1);
}

int main()
{
    signal(SIGINT,int_handler);
    sigset_t set;
    sigset_t oset;
    sigset_t saveset;
    sigemptyset(&set); // 清空集合
    sigaddset(&set,SIGINT); // 把SIGINT信号放入信号集中
    // 保存之前的状态
    sigprocmask(SIG_UNBLOCK,&set,&saveset);
    sigprocmask(SIG_BLOCK,&set,&oset); // 把信号集里的所有信号BLOCK阻塞
    for (int i = 0; i < 1000; i++)
    {
        // 这里面的代码不再受信号响应
        for (int j = 0; j < 5; j++)
        {
            write(1,"*",1);
            sleep(1);
        }
        
        
        // sigset_t tmpset;
        // sigprocmask(SIG_SETMASK,&oset,&tmpset); // 解除阻塞
        // pause();
        // sigprocmask(SIG_SETMASK,&tmpset,NULL);
        
        // oset是非阻塞的，set是阻塞的，这句话是以非阻塞等待信号，有信号来且处理完毕之后，
        // 再回复以前的屏蔽字，也就是set，自然也就恢复为阻塞了。所以它只能处理一个信号，因为是原子操作
        sigsuspend(&oset); //这句话相当于上面三句的原子操作
        write(1,"\n",1);
    }
    // 恢复为模块之前的状态
    sigprocmask(SIG_SETMASK,&saveset,NULL);
    exit(0);
}