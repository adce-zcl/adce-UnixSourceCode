/**
 * @file snder.c
 * @author your name (you@domain.com)
 * @brief 发送方 -- 主动端
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
#include <string.h>
int main()
{
    key_t key;
    int msgid;
    struct msg_st sbuf;

    key = ftok(KEYPATH,KEYPROJ);
    if(key == -1)
    {
        perror("ftok()");
        exit(1);
    } 
    
    msgid = msgget(key,0);           // 机制已经被创建，只要能拿到同一个就行
    if(msgid < 0)
    {
        perror("msgget()");
        exit(1);
    }
    
    sbuf.mtype = 1;             // 不能小于0就可以
    strcpy(sbuf.name,"Alan");
    sbuf.math = rand()%100;
    sbuf.chinese = rand()%100;

    if(msgsnd(msgid,&sbuf,sizeof(sbuf)-sizeof(long),0) < 0)
    {
        perror("msgsnd()");
        exit(1);
    }
    #if 0
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
    #endif
    puts("OK");
    // 不无要销毁，因为不是我创建的
    exit(0);
}