/**
 * @file 2-mqsetattr.c
 * 设置或修改消息队列的属性
 * 属性有四项：flag, maxmsg, msgsize, curmsgs;
 * 其中：
 * 1.消息队列的最大容量和每个消息的最大长度只能在创建的时候设置,手动检查是否为0,不能超过规定值（文件中）
 * 2.flag只有是否阻塞两个选项，可以随意随时设置
 * 3.当前消息队列的消息的个数，任何时候不能设置，只能获取
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MQNAME "/temp.1"
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)
int main(int argc, char **agrv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Using...\n");
        exit(0);
    }
    mqd_t mqd;
    struct mq_attr attr;
    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg = atol(agrv[1]);
    attr.mq_msgsize = atol(agrv[2]);
    mqd = mq_open(MQNAME, O_RDWR | O_CREAT | O_EXCL, MODE, &attr);
    if (mqd == (mqd_t)-1)
    {
        perror("mq_open");
        exit(-1);
    }
    mq_close(mqd);
    exit(0);
}