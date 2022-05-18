/**
 * @file 3-mqreceive.c
 * 接受消息，阻塞
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <strings.h>
#define MQNAME "/temp.1234"
int main(int argc, char **argv)
{
    char *buf;
    long buf_len;
    mqd_t mqd;
    struct mq_attr attr;
    // 打开消息队列
    mqd = mq_open(MQNAME, O_RDONLY);
    if (mqd == (mqd_t)-1)
    {
        perror("mq_open");
        exit(-1);
    }

    // 获取消息队列的属性
    bzero(&attr, sizeof(attr));
    mq_getattr(mqd, &attr);
    buf_len = attr.mq_msgsize;

    // 给buf申请内存
    buf = (char *)malloc(buf_len);
    if (buf == NULL)
    {
        fprintf(stderr, "malloc\n");
        mq_close(mqd);
        exit(-1);
    }

    // 接受消息
    // prio == 0, 表示接受优先级最高的，可以指定优先级
    // 如果指定的优先级不存在，就接受最高优先级
    unsigned int prio = 20;
    bzero(buf, buf_len);
    mq_receive(mqd, buf, buf_len, &prio);
    printf("msg:%s\n", buf);
    // bzero(buf, buf_len);
    // mq_receive(mqd, buf, buf_len, &prio);
    // printf("msg:%s\n", buf);

    exit(0);
}