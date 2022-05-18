/**
 * @file rcver.c
 * @author your name (you@domain.com)
 * @brief 接收方 -- 被动端
 * @version 0.1
 * @date 2022-02-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main()
{
    key_t key;
    int msgid;
    struct msg_st rbuf;

    key = ftok(KEYPATH,KEYPROJ);
    if(key == -1)
    {
        perror("ftok()");
        exit(1);
    }
    // IPC_CREAT 使其先创建起来
    msgid = msgget(key,IPC_CREAT | 0600);           // 0600代表权限，一定要设置
    if(msgid < 0)
    {
        perror("msgget()");
        exit(1);
    }

    while(1)
    {
        if(msgrcv(msgid,&rbuf,sizeof(rbuf)-sizeof(long),0,0) < 0)
        {
            perror("msgrcv()");
            exit(1);
        }
        printf("NAME = %s\n",rbuf.name);
        printf("MATH = %d\n",rbuf.math);
        printf("CHINESE = %d\n",rbuf.chinese);
    }
    
    megctl(msgid,IPC_RMID,NULL);
    exit(0);
}