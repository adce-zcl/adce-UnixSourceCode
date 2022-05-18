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
    for (int i = 0; i < 1000; i++)
    {
        sigprocmask(SIG_BLOCK,&set,&oset); // 把信号集里的所有信号BLOCK阻塞
        // 这里面的代码不再受信号响应
        for (int j = 0; j < 5; j++)
        {
            write(1,"*",1);
            sleep(1);
        }
        sigprocmask(SIG_SETMASK,&oset,NULL); // 解除阻塞,开始响应信号，但只相应一个
        write(1,"\n",1);
    }
    // 恢复为模块之前的状态
    sigprocmask(SIG_SETMASK,&saveset,NULL);
    exit(0);
}