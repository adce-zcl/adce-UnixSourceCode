/**
 * @file mqsend.c
 * 往消息队列上发送消息,消息有优先级
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#define MQNAME "/temp.1234"
#define MSG1 "this is a msg, prio = 10."
#define MSG2 "this is a msg, prio = 20."
int main(int argc, char **argv)
{
    char *buf;
    size_t buf_len = 1024;
    unsigned int prio1 = 10;
    unsigned int prio2 = 20;
    mqd_t mqd;

    // 打开消息队列
    mqd = mq_open(MQNAME, O_RDWR);
    if (mqd == (mqd_t)-1)
    {
        perror("mq_open");
        exit(-1);
    }

    // 申请缓冲区内存
    buf = (char *)malloc(buf_len);
    if (buf == NULL)
    {
        fprintf(stderr, "malloc\n");
        exit(-1);
    }

    
    // 发送
    strncpy(buf, MSG1, sizeof(MSG1));
    if (mq_send(mqd, buf, buf_len, prio1) < 0)
    {
        perror("mq_send");
        exit(-1);
    }
    // strncpy(buf, MSG2, sizeof(MSG2));
    // if (mq_send(mqd, buf, buf_len, prio2) < 0)
    // {
    //     perror("mq_send");
    //     exit(-1);
    // }

    free(buf);
    mq_close(mqd);
    exit(0);
}