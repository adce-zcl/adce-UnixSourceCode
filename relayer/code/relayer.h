/**
 * @brief 中继引擎
 * 
 */
#ifndef _RELAYER_H_
#define _RELAYER_H_

/**
 * @brief 任务的状态
 * 
 */
enum
{
    STATE_RUNNING = 1,
    STATE_CANCELED,
    STATE_OVER
};

/**
 * @brief 任务状态结构体
 * 状态是枚举类型
 * 给用户看的
 */
struct rel_stat_st
{
    int state;
    int fd1;
    int fd2;
    // long int count12,count21; //对话多少字节
    // struct timerval start,end;
};


/**
 * @brief 往数组里添加job
 * @param fd1 
 * @param fd2 
 * @return int 
 * return >= 0          成功，返回当前任务ID
 * return == -EINVAL    失败，参数非法
 * return == -ENOSPC    失败，任务数组满
 * return == -ENOMEM    失败，内存分配错误
 */
int rel_addjob(int fd1,int fd2);

/**
 * @brief 取消任务
 * 
 * @param id 
 * @return int 
 * return   == 0            成功，制定任务成功取消
 *          == -EINVAL      失败，参数非法
 *          == -EBUSY       失败，任务重复取消
 */
int rel_canceljob(int id);

/**
 * @brief 收尸函数
 * 
 * @param id 
 * @param rel 
 * @return int 
 * return   ==  0       成功，指定任务已终止并返回状态
 *          ==  -EINVAL 失败，参数非法
 */
int rel_waitjob(int id, struct rel_stat_st *rel);

/**
 * @brief 填充某个id的任务状态
 * 
 * @param id 
 * @param rel 
 * @return int 
 * return   ==  0       成功，指定任务状态已经返回
 *          ==  -EINVAL 失败，参数非法
 */
int rel_statjob(int id, struct rel_stat_st *rel);

#endif