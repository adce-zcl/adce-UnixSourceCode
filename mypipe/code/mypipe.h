/**
 * @file mypipe.h
 * @author your name (you@domain.com)
 * @brief 管道其实和队列很像，就是一种机制
 * 这个例子是人为实现的管道，跟系统提供的还是有一点差距
 * @version 0.1
 * @date 2022-02-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _MYPIPE_H_
#define _MYPIPE_H_
#define PIPESIZE 1024               // 管道的大小
#define MYPIPE_READ 0x00000001UL    // 代表身份
#define MYPIPE_WRITE 0x00000002UL   // 专业的位图
struct mypipe_st                    // 把真正的数据结构封装，并且隐藏起来
{
    int head;
    int tail;
    char data[PIPESIZE];
    pthread_mutex_t mut;            // 互斥量
    pthread_cond_t cond;            // 条件变量
    int datasize = 0;
    int count_rd = 0;                   // 读者计数
    int count_wt = 0;
};
typedef void mypipe_t;              // 隐藏真实的数据结构
mypipe_t* mypipe_init(void);        // 初始化函数
int mypipe_destory(mypipe_t*);      // 销毁函数
int mypipe_read(mypipe_t*,void *buf,size_t size);
int mypipe_write(mypipe_t*,const void* buf,size_t size);
int mypipe_register(mypipe_t *,int opmap);              // 注册身份
int mypipe_unregister(mypipe_t*,int opmap);             // 注销身份


#endif