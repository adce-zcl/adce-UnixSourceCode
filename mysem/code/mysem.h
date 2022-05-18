#ifndef _MYSEM_H_
#define _MYSEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

typedef void mysem_t;
/**
 * @brief 
 * 资源初始化
 * @param initval 
 * @return mysem_t* 
 */
mysem_t* mysem_init(int initval);

/**
 * @brief 资源的归还与释放
 * 
 * @return int 
 */
int mysem_add(mysem_t*,int);
int mysem_sub(mysem_t*,int);

/**
 * @brief 销毁资源量，返回是否成功
 * 
 * @return int 
 */
int mysem_destory(mysem_t *);


#endif