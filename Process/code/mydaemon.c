/**
 * @file mydaemon.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-10
 * 
 * @copyright Copyright (c) 2022
 * 
 * 守护进程
 * 这个进程要从终端打开才会叫守护进程
 * 不然的话，他的父进程会变成vscode的进程
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#define FILENAME "/home/zhangchenglei/vscode/Process/file/daemon.txt"
// 守护进程变成规则
/**
 * 1.umask(0)，放大守护进程的文件权限
 * 2.fork()，exit父进程，使得子进程变成孤儿进程，由init进程接管，同时保证了子进程不会是一个组的组长进程
 * 3.子进程setsid()创建会话，只有非组长进程才能创建会话
 * 4.更改子进程工作目录，一般是"/"根目录
 * 5.关闭不再需要的文件描述符，可以通过getrlimit()来判断当前持有的最高的文件描述符，并依次关闭
 * 
 * @return int 
 */
static int daemonize()
{
    pid_t pid = fork();
    if(pid < 0)
    {
        // perror("fork()");
        return -1;
    }
    if(pid > 0)
    {
        // 只能由子进程升级成守护进程，
        // 升级之后，子进程的父进程要关闭
        exit(0);
    }
    int fd = open("/dev/null",O_RDWR);
    if(fd < 0)
    {
        // 这里要写入系统日志，而不是终端
        // perror("fopen()");
        return -1;
    }

    // 调用setsid之前，对012进行重定向
    // 这是什么意思？ 关闭0，1，2文件
    // 为什么把0，1，2重定向到null描述符，而不是直接关掉？
    dup2(fd,0);
    dup2(fd,1);
    dup2(fd,2);
    if(fd > 2)
    {
        close(fd);
    }

    // 升级
    setsid();

    // 收尾
    // 把工作路径设置为根路径
    chdir("/");
    return 0;
}

int main()
{
    openlog("zcl daemon",LOG_PID,LOG_DAEMON);
    if(daemonize() == -1)
    {
        syslog(LOG_ERR,"daemonize() failed!");
        exit(1);
    }
    else
    {
        syslog(LOG_INFO,"daemonize() successd!");
    }
    FILE *fp = fopen(FILENAME,"w");
    if(fp == NULL)
    {
        syslog(LOG_ERR,"fopen():%s",strerror(errno));
        exit(1);
    }
    else
    {
        syslog(LOG_INFO,"fopen() successd!");
    }
    
    for (int i = 0;; i++)
    {
        fprintf(fp,"%d\n",i);
        fflush(fp);
        syslog(LOG_DEBUG,"%d had fflush!",i);
        sleep(1);
    }
    // 这是异常终止，因为永远不会执行到下面两条
    fclose(fp);
    closelog();
    exit(0);
}