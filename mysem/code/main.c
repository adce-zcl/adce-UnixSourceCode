#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "mysem.h"
#define LEFT 30000000
#define RIGHT 30000200
#define THRNUM (RIGHT-LEFT+1) //线程总量
#define N 4 // 同一时刻的最大的线程数量
static void* thr_prime(void *p);
static mysem_t *sem;
int main()
{
    pthread_t tid[THRNUM];
    sem = mysem_init(N);
    if(sem == NULL){
        fprintf(stderr,"mysem_init() faile!\n");
        exit(1);
    }

    for(int i = LEFT; i < RIGHT; i++){
        mysem_sub(sem,1);
        int err = pthread_create(tid+(i-LEFT),NULL,thr_prime,(void*)i);
        if(err){
            fprintf(stderr,"pthread_create():%s\n",strerror(err));
            exit(1);
        }
    }
    for (int i = LEFT; i <= RIGHT; i++){
        pthread_join(tid[i-LEFT],NULL);
    }
    mysem_destory(sem);

    exit(0);
}
static void* thr_prime(void *p){
    int i,j,mark;
    mark = 1;
    i = (int)p;
    for ( j = 2; j < i/2; j++){
        if(i%j == 0){
            mark = 0;
            break;
        }
    }
    if(mark){
        printf("%d is a primer\n",i);
    }

    sleep(500);
    mysem_add(sem,1);
    pthread_exit(NULL);
    
}