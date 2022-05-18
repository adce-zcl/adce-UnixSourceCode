/**
 * @file 4-1mqnotify.c
 * 简单的信号通知
 * 
 * 使用消息队列异步通知
 * 1.mq_notify是一次性的,想要多次使用，必须重复注册
 * 2.任意时刻任何进程，只能有一个注册接受通知
 * 3.空指针代表注销通知
 * 4.receive的阻塞比通知优先级高
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <mqueue.h>
#include <unistd.h>
#define MQNAME "/temp.1234"

mqd_t mqd;
void *buf;
long buf_len;
struct mq_attr attr;
struct sigevent sigev;

/**
 * 信号处理函数
 */
static void sig_usr(int p)
{
    // 重新注册
    mq_notify(mqd, &sigev);

    // 收到信号就去消息队列里取消息
    mq_receive(mqd, (char*)buf, buf_len, NULL);
    printf("msg:%s\n", buf);

    // 清空
    bzero(buf, buf_len);

    return;
}
    
int main(int argc, char **argv)
{
    // 打开消息队列
    mqd = mq_open(MQNAME, O_RDWR);
    if (mqd == (mqd_t)-1)
    {
        perror("mq_open");
        exit(-1);
    }

    // 获取属性
    mq_getattr(mqd, &attr);

    // 获取最大size
    buf_len = attr.mq_msgsize;

    // buf申请内存
    buf = (void *)malloc(buf_len);
    if (buf == NULL)
    {
        fprintf(stderr, "malloc\n");
        exit(-1);
    }

    // 注册信号
    signal(SIGUSR1, sig_usr);

    // 设置信号响应
    bzero(&sigev, sizeof(sigev));
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;

    // 注册通知者
    if (mq_notify(mqd, &sigev) < 0)
    {
        perror("mq_notify");
        exit(-1);
    }

    for ( ;; )
    {
        sleep(1);
    }

    exit(0);
}