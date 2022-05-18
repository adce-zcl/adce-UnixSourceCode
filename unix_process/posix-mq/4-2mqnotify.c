/**
 * @file 4-2mqnotify.c
 * 非阻塞的mq_notify的信号通知
 * 使用阻塞信号临界区
 * 注意信号的很多东西得是gnuc标准
 */
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
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
        // 阻塞信号，形成信号临界区代码,这个函数不能在多线程中使用，因为我们应该只改变当前线程的掩码
        sigprocmask(SIG_BLOCK, &newmask, &oldmask);

        // 这中间得代码不能被信号打断，来的信号全部排队阻塞

        // 从阻塞队列里取出一个信号处理
        while (mqflag == 0)
        {
            // 暂停阻塞，暂停线程，直到某个某个信号从处理函数返回
            // 相当用从信号临界区，取出一个阻塞的信号，原子地等待它的处理函数执行完毕
            // 然后再将临界区封闭，继续阻塞信号
            sigsuspend(&zeromask);
        }

        // 重新设置等待条件
        mqflag = 0;
        mq_notify(mqd, &sigev);

        // 读数据
        ssize_t n;
        while ((n = mq_receive(mqd, (char*)buf, attr.mq_msgsize, NULL)) >= 0)
        {
            printf("msg:%s\n", buf);
        }

        // 判断错误,因为mqd是非阻塞的，所以如果的读的时候没有数据，就以EAGAIN错误返回
        // 但是这个错误不是程序错误
        if (errno != EAGAIN) 
        {
            perror("mq_receive");
            mq_close(mqd);
            exit(-1);
        }

        // 接触阻塞，临界区代码下边界
        sigprocmask(SIG_UNBLOCK, &newmask, NULL);
    }
    exit(0);
}
void sig_user1(int p)
{
    mqflag = 1;
    return;
}