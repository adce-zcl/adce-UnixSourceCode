#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "relayer.h"

/**
 * @brief 状态机的状态
 * 
 */
enum
{
    STATE_R = 1, // 读态
    STATE_W, // 写态
    STATE_Ex, // 异常处理态
    STATE_T // 终止态
};

/**
 * @brief 状态机的描述
 * 
 */
struct rel_fsm_st
{
    int state;
    int sfd;
    int dfd;
    // 缓冲区
    char buf[BUFSIZ];
    int len; // 读了多少字节
    int pos; // 标记写的位置
    char *errstr; // 出错的字符串
    long int count; // 记录传输的字节数量
};

/**
 * @brief 工作的结构体
 * 
 */
struct rel_job_st
{
    int fd1;
    int fd2;
    int job_state;
    struct rel_fsm_st fsm12,fsm21;
    int fd1_save,fd2_save; // 保留状态
};

// job数组的最大值,有多个状态机同时推进的时候，这个数组必须保持唯一性
#define REL_JOBMAX 10000
static struct rel_job_st* rel_job[REL_JOBMAX];
static pthread_mutex_t mut_rel_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

#if 1

static void fsm_driver(struct rel_fsm_st *fsm)
{
    switch(fsm->state)
    {
        case STATE_R:
        {
            fsm->len = read(fsm->sfd,fsm->buf,BUFSIZ);
            if(fsm->len == 0)
            {
                fsm->state = STATE_T;
            }
            else if(fsm->len < 0)
            {
                // 判断真错还是假错
                if(errno == EAGAIN) // 假错
                {
                    
                    fsm->state = STATE_R;
                }
                else // 真错
                {
                    fsm->errstr = "read()";
                    fsm->state = STATE_Ex;
                }
            }
            else
            {
                fsm->pos = 0;
                fsm->state = STATE_W;
            }
            break;
        }
        case STATE_W:
        {
            int ret = write(fsm->dfd,fsm->buf+fsm->pos,fsm->len);
            if(ret < 0) // 写入失败
            {
                if(errno == EAGAIN) // 假错
                {
                    fsm->state = STATE_W;
                }
                else // 真错
                {
                    fsm->errstr = "write()";
                    fsm->state = STATE_Ex;
                }
            }
            else // 成功
            {
                fsm->pos += ret;
                fsm->len -= ret;
                if(fsm->len == 0) //写完了
                {
                    fsm->state = STATE_R;
                }
                else // 没写完，继续推动到写态
                {
                    fsm->state = STATE_W;
                }
            }
            break;
        }
        
        case STATE_Ex:
        {
            perror(fsm->errstr);
            fsm->state = STATE_T; //推动到终止态
            break;
        }
        case STATE_T:
        {
            /* do smoething*/
            break;
        }
        default:
        {
            abort(); // 发送一个信号
            // 程序对信号的默认相应是关闭，所以程序到这里可能会被关闭
            break;
        }
    }
}

#endif

/**
 * @brief 线程函数，找到可以推动的有限机器
 * 
 * @param p 
 * @return void* 
 */
static void* thr_relayer(void* p)
{
    while (1)
    {
        pthread_mutex_lock(&mut_rel_job);
        for (int i = 0; i < REL_JOBMAX; i++)
        {
            if(rel_job[i] != NULL)
            {
                if(rel_job[i]->job_state == STATE_RUNNING)
                {
                    fsm_driver(&rel_job[i]->fsm12);
                    fsm_driver(&rel_job[i]->fsm21);

                    if(rel_job[i]->fsm12.state == STATE_T && \
                        rel_job[i]->fsm21.state == STATE_T)
                    {
                        rel_job[i]->job_state = STATE_OVER;
                    }
                }
            }
        }
        pthread_mutex_unlock(&mut_rel_job);
    }
}


/**
 * @brief 创建一个线程，永远推状态机
 * 
 */
static void module_load(void)
{
    pthread_t tid_relayer;
    int err = pthread_create(&tid_relayer,NULL,thr_relayer,NULL);
    if(err)
    {
        fprintf(stderr,"pthread_create():%s\n",strerror(err));
        exit(1);
    }
}

/**
 * @brief 释放空间
 * 
 */
static void module_unload(void)
{

}

int get_free_pos_unlock()
{
    for (int i = 0; i < REL_JOBMAX; i++)
    {
        if(rel_job[i] == NULL)
        {
            return i;
        }
    }
    return -1;
    
}

int rel_addjob(int fd1,int fd2)
{
    // 只调用一次 -- 动态模块的单次处理化的实现
    pthread_once(&init_once,module_load); 

    struct rel_job_st *me;
    me = malloc(sizeof(*me));
    if(me == NULL)
    {
        return -ENOMEM;
    }
    me->fd1 = fd1;
    me->fd2 = fd2;
    me->job_state = STATE_RUNNING;
    me->fd1_save = fcntl(me->fd1,F_GETFL);
    fcntl(me->fd1,F_SETFL,me->fd1_save | O_NONBLOCK);
    me->fd2_save = fcntl(me->fd2,F_GETFL);
    fcntl(me->fd2,F_SETFL,me->fd2_save | O_NONBLOCK);
    me->fsm12.sfd = me->fd1;
    me->fsm12.dfd = me->fd2;
    me->fsm12.state = STATE_R;
    me->fsm21.sfd = me->fd2;
    me->fsm21.dfd = me->fd1;
    me->fsm21.state = STATE_R;
    
    // 找空位
    pthread_mutex_lock(&mut_rel_job);
    int pos = get_free_pos_unlock(); //unlock版本
    if(pos < 0)
    {
        pthread_mutex_unlock(&mut_rel_job);
        fcntl(me->fd1,F_SETFL,me->fd1_save);
        fcntl(me->fd2,F_SETFL,me->fd2_save);
        free(me);
        return -ENOSPC;
    }
    rel_job[pos] = me;
    pthread_mutex_unlock(&mut_rel_job);
    return pos;
}

#if 0
int rel_canceljob(int id)
{

}

int rel_waitjob(int id, struct rel_stat_st *rel)
{

}

int rel_statjob(int id, struct rel_stat_st *rel)
{

}
#endif