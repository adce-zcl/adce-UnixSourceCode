/**
 * @file 4-5mqnotify.c
 * 使用mq_notify()的正统用法——调用线程
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <mqueue.h>
#define MQNAME "/temp.1234"

mqd_t mqd;
struct mq_attr attr;
struct sigevent sigev;

static void notify_thread(union sigval sv)
{
    int n;
    void *buf;
    buf = malloc(attr.mq_msgsize);
    if (buf == NULL)
    {
        fprintf(stderr, "malloc failed.\n");
        exit(-1);
    }
    mq_notify(mqd, &sigev);

    while ((n = mq_receive(mqd, (char *)buf, attr.mq_msgsize, NULL)) >= 0)
    {
        printf("msg:%s\n", buf);
    }
    if (errno != EAGAIN)
    {
        perror("mq_receive");
        exit(-1);
    }
    free(buf);
    pthread_exit(NULL);
}

int main()
{
    mqd = mq_open(MQNAME, O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t)-1)
    {
        perror("mq_open");
        exit(-1);
    }

    mq_getattr(mqd, &attr);

    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = NULL;
    sigev.sigev_notify_function = notify_thread;
    sigev.sigev_notify_attributes = NULL;

    mq_notify(mqd, &sigev);
    
    while (1)
    {
        pause();
    }
    exit(0);
}