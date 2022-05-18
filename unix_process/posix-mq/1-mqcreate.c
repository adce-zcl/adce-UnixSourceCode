/**
 * @file 1-mqcreate.c
 * 创建一个消息队列
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
// 注意Posxi消息队列的命名只能是/queuename.
// 不可以用路径，默认都是放在/dev/mqueue里
#define MQNAME "/temp.1234"
// 创建的文件的权限
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)
int main(int argc, char **argv)
{
    mqd_t mqd;
    mqd = mq_open(MQNAME, O_RDWR | O_CREAT, MODE, NULL);
    if (mqd == (mqd_t)-1)
    {
        perror("mq_open");
        exit(-1);
    }
    mq_close(mqd);
    // 不加这句，temp.1234 不会被删除，这了这句话，就会被删除
    // mq_unlink(MQNAME);
    exit(0);
}