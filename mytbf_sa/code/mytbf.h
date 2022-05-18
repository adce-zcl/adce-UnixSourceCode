#ifndef __MYTBF_H__
#define __MYTBF_H__
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
// 最多能支持1024个不同速率的令牌桶
#define MYTBF_MAX 1024
typedef void mytbf_t;

mytbf_t *mytbf_init(int cps,int burst);
int mytbf_fetchtoken(mytbf_t*,int );
int mytbf_returntoken(mytbf_t*, int);

int mytbf_destory(mytbf_t*);




#endif