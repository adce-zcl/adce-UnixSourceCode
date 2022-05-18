/**
 * @file 4-3mqnotify.c
 * 使用sigwait代替sigsuspend
 * 注意，我们一直使用的是signal
 * 这个函数是有缺陷的，因为它会在执行一个信号处理函数的时候，被另一个信号打断而跑去执行另一个，
 * 虽然可以返回来，但是返回的环境可能会不同。
 * 如果不想信号在处理函数中被别的信号打断，应该使用sigaction()函数。
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#define MQNAME "/temp.1234"
int main(int agrc, char **argv)
{
    mqd_t mqd;
    void *buf;
    struct mq_attr attr;

    mqd = mq_open(MQNAME, O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t)-1)
    {
        perror("mq_open");
        exit(-1);
    }

    mq_getattr(mqd, &attr);

    buf = malloc(attr.mq_msgsize);
    if (buf == NULL)
    {
        fprintf(stderr, "malloc failed.\n");
        exit(-1);
    }

    sigset_t newmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);

    sigprocmask(SIG_BLOCK, &newmask, NULL);

    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;

    mq_notify(mqd, &sigev);

    while (1)
    {
        int signo;
        // 这个参数是信号集，信号集和信号屏蔽字本质都是sigset_t类型的位图，这里信号集中只有SIGUSR1
        // 而做阻塞用的时候，也是只阻塞SIGUSR1
        // 这个函数可以用在多线程里，因为它执行的时候会暂时挂起线程，只返回响应的信号，并把信号编号填充signo
        sigwait(&newmask, &signo);

        if (signo == SIGUSR1)
        {
            // 读
            mq_notify(mqd, &sigev);
            int n;
            while ((n = mq_receive(mqd, (char *)buf, attr.mq_msgsize, NULL)) >= 0)
            {
                printf("msg:%s\n", buf);
            }
            if (errno != EAGAIN)
            {
                perror("mq_receive");
                exit(-1);
            }
        }
    }
}