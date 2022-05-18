#ifndef __ANYTIME_H__
#define __ANYTIME_H__
#define JOB_MAX 1024
typedef void at_jobfunc_t(void*);

/**
 * @brief 
 * 
 * @param sec 
 * @param jobp 
 * @param arg 
 * @return int 
 * return >= 0 成功，返回任务ID
 * return == -EINVAL 失败，参数非法
 * return == -ENOSPC 失败，数组满
 * return == -ENOMEM 失败，内存空间不足
 */
int at_addjob(int sec, at_jobfunc_t *jobp, void *arg);

/**
 * @brief 
 * 
 * @param id 
 * @return int 
 * return == 0 成功，指定任务成功取消
 * return == -EINVAL 失败，参数非法
 * return == -EBUSY 失败，指定任务已经完成
 * return == -ECANCELED 失败，指定任务重复取消
 * 
 * 进程的wait的操作，是典型的异步操作同步化
 */
int at_canceljob(int id);

/**
 * @brief 
 * 
 * @param id 
 * @return int 
 * 
 * return == 0 成功，指定任务成功释放
 * return == -EINVAL， 失败，参数非法
 */
int at_waitjob(int id);

// at_pausejob(int id);
// at_resumejob(int id);

#endif