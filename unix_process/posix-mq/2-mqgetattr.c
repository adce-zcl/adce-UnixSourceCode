/**
 * @file 2-mqgetattr.c
 * 获取消息队列的属性
 * 属性有四项：flag, maxmsg, msgsize, curmsgs;
 * 其中：
 * 1.消息队列的最大容量和每个消息的最大长度只能在创建的时候设置,手动检查是否为0
 * 2.flag只有是否阻塞两个选项，可以随意随时设置
 * 3.当前消息队列的消息的个数，任何时候不能设置，只能获取
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#define MQNAME "/temp.1"
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)
int main(int argc, char **agrv)
{
    mqd_t mqd;
    struct mq_attr attr;
    mqd = mq_open(MQNAME, O_RDWR, MODE, NULL);
    mq_getattr(mqd, &attr);
    printf("flag:%ld\tmaxmsg:%ld\tmsgsize:%ld\tcurmsgs:%ld\n", attr.mq_flags, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
    mq_close(mqd);
    exit(0);
}