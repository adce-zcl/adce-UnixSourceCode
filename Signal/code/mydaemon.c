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
#include <signal.h>
#include <sys/mman.h>
#define FILENAME "/home/zhangchenglei/vscode/Process/file/daemon.txt"
FILE *fp;
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
    // 这是什么意思？
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

static void daemon_exit(int s)
{
    // 阻塞其他信号
    
    fclose(fp);
    closelog();
    exit(0);
}

int main()
{

    // signal(SIGINT,daemon_exit);
    // signal(SIGQUIT,daemon_exit);
    // signal(SIGTERM,daemon_exit);
    struct sigaction sa;
    // 规定信号处理函数
    sa.sa_handler = daemon_exit;
    sigemptyset(&sa.sa_mask);
    // 规定哪些信号阻塞
    sigaddset(&sa.sa_mask,SIGQUIT);
    sigaddset(&sa.sa_mask,SIGTERM);
    sigaddset(&sa.sa_mask,SIGINT);
    // 规定特殊标志
    sa.sa_flags = 0;
    // 规定处理哪些信号
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGQUIT,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);

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
    fp = fopen(FILENAME,"w");
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
}