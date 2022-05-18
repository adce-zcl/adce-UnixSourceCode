/**
 * @file 4-2mqnotify.c
 * 非阻塞的mq_notify的信号通知
 * 使用阻塞信号临界区
 */
#include <signal.h>
#include <unistd.h>
#include <stdiao.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <mqueue.h>
#define MQNAME "/temp.1234"
volatile sig_atomic_t mqflag;
static void sig_user1(int p);

int main(int argc, char **argv)
{
    mqd_t mqd;
    void *buf;
    sigset_t zeromask, newmask, oldmask;
    struct mq_attr attr;
    struct sigevent sigev;

    // 非阻塞打开消息队列
    mqd = mq_open(MQNAME, O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t)-1)
    {
        perror("mq_open");
        exit(-1);
    }

    // 获取消息队列的属性
    mq_getattr(mqd, &attr);

    // 给存储区申请内存
    buf = malloc(attr.mq_msgsize);

    // 清空信号屏蔽字
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigemptyset(&oldmask);

    // 添加阻塞信号
    sigaddset(&newmask, SIGUSR1);

    // 注册信号处理函数
    signal(SIGUSR1, sig_user1);

    // 设置通知信号
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;

    // 注册信号通知
    mq_notify(mqd, &sigev);

    // 循环等待信号
    while (1)
    {
        // 按照信号屏蔽字，阻塞信号
        sigprocmask(SIG_BLOCK, &newmask, &oldmask);

        // 从阻塞队列里取出一个信号处理
        while (mqflag == 0)
        {
            sigprocmask(SIG_BLOCK, )            
        }
        
    }
}
void sig_user1(int p)
{

}