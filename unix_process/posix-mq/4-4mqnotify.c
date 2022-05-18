/**
 * @file 4-4mqnotify.c
 * 使用select
 * 因为mqd_t文件描述符不是普通的文件描述符，不能直接用于select
 * 所以我们需要桥接，管道描述符可以用与select，可以使用管道描述符和信号进行桥接
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#define MQNAME "/temp.1234"
int pipefd[2];
static void sig_usr(int p)
{
    // 写的目的就是为了触发select的活动
    write(pipefd[1], " ", 1);
    return;
}

int main(int argc, char **argv)
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

    // 新建一个管道
    pipe(pipefd);

    signal(SIGUSR1, sig_usr);

    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    mq_notify(mqd, &sigev);

    // 文件描述符集，防止需要select监听的描述符
    fd_set fset;
    // 先清空
    FD_ZERO(&fset);

    while (1)
    {
        // 把管道的读端添加到文件描述符集合
        FD_SET(pipefd[0], &fset);

        // 最大描述符， 监听读，监听写，监听错误，时间
        select(pipefd[0] + 1, &fset, NULL, NULL, NULL);

        // 检查响应的是不是pipefd[0]
        // 活动的描述符被保留，未活动的会被select设置为0，所以每次循环都要重置fset
        // 同样这个fset也会被整个拷贝道内核中
        if(FD_ISSET(pipefd[0], &fset))
        {
            char ch;
            read(pipefd[0], &ch, 1);    // 把标记字符读出来

            mq_notify(mqd, &sigev);

            int n;
            while ((n = mq_receive(mqd, (char*)buf, attr.mq_msgsize, NULL)) >= 0)
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
    exit(0);
}