#include "mypipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
mypipe_t* mypipe_init(void)
{
    struct mypipe_st *me;
    me = malloc(sizeof(*me));
    if(me == NULL)
    {
        return NULL;
    }
    me->head = 0;
    me->tail = 0;
    me->datasize = 0;
    pthread_mutex_init(&me->mut,NULL);
    pthread_cond_init(&me->cond,NULL);
    return me;
}
int mypipe_destory(mypipe_t* ptr)
{
    struct mypipe_st *me = ptr;
    pthread_mutex_destroy(&me->mut);
    pthread_cond_destroy(&me->cond);
    free(ptr);
    return 0;
}

int mypipe_register(mypipe_t *ptr,int opmap)    // 注册身份
{
    /*如果最低位和次低位不是为1,报错*/
    struct mypipe_st *me = ptr;
    pthread_mutex_lock(&me->mut);
    if(opmap & MYPIPE_READ)
    {
        me->count_rd++;
    }
    if(opmap & MYPIPE_WRITE)
    {
        me->count_wt++;
    }
    pthread_cond_broadcast(&me->cond);
    // 判断是否满足读写双方，管道只有在凑齐了读写双方才能使用
    while (me->count_rd <= 0 || me->count_wt <= 0)
    {
        pthread_cond_wait(&me->cond,&me->mut);
    }
    pthread_mutex_unlock(&me->mut);
    return 0;
}

int mypipe_unregister(mypipe_t *ptr,int opmap)
{
    struct mypipe_st *me = ptr;
    pthread_mutex_lock(&me->mut);

    if(opmap & MYPIPE_READ)
    {
        me->count_rd--;
    }
    if(opmap & MYPIPE_WRITE)
    {
        me->count_wt--;
    }
    pthread_cond_broadcast(&me->cond);
    pthread_mutex_unlock(&me->mut);
    return 0;
}

static int next(int a)
{

}

static int mypipe_readbyte_unlock(struct mypipe_st* me,char *datap)
{
    if(me->datasize <= 0)
    {
        return -1;
    }
    *datap = me->data[me->head];
    me->head = next(me->head);
    me->datasize--;
    return 0;
}

int mypipe_read(mypipe_t* ptr,void *buf,size_t count)
{
    struct mypipe_st *me = ptr;
    pthread_mutex_lock(&me->mut);
    while (me->datasize <= 0 && me->count_wt > 0)
    {
        pthread_cond_wait(&me->cond,&me->mut);
    }
    if(me->datasize <= 0 && me->count_wt < 0)
    {
        pthread_mutex_unlock(&me->mut);
        return 0;
    }
    int i;
    for (i = 0; i < count; i++)
    {
        if(mypipe_readbyte_unlock(me,buf+i) != 0) // 一次读一个字节
        {
            break;
        }
    }
    pthread_cond_broadcast(&me->cond);              // 通知来写
    pthread_mutex_unlock(&me->mut);
    return i;
}
int mypipe_write(mypipe_t *ptr, const void* buf,size_t size)
{
    
}