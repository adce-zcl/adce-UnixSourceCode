#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <time.h>
#include "proto.h"
#define MINSPARESERVER 5                        // 最小的支持的空闲的个数
#define MAXSPARESERVER 10                       // 最大的空闲的个数
#define MAXCLIENTS 20                           // 最多同时支持20个客户端连接
#define SIG_NOTIFY SIGUSR2                      // 自定义信号，SIGUSR1,2都是预留给我们自己定义的信号
#define LINEBUFSIZE 1024
#define IPSTRSIZE 40
enum                                            // 子进程的状态
{       
    STATE_IDEL=0,                               // 空闲
    STATE_BUSY                                  // 忙碌
};      
struct server_st                                // 服务器端
{       
    pid_t pid;                                  // 负责当前服务器的进程id
    int state;                                  // 子进程的状态
};      
static struct server_st *serverpool;            // 进程池
static int idle_count = 0, busy_count = 0;      // 记录数量

static void setsignalwait(void);                // 固定功能，自动给子进程收尸
static void setsignalmy(void);                  // 给自定义信号绑定功能
static void setsignalmy_set(sigset_t *set,sigset_t *oldset);
static void usr2_handler(int s);                // 自定义信号功能函数
static void poolmemcreate(void);                // 给进程池指针创建内存
static int socketcreate(void);                  // 建立监听socket
static void setsocketoptmy(int *listensocket);  // 设置socket属性
static void bindmy(int *listensocket);          // 绑定函数
static void listenmy(int *listensocket);        // 设置监听
static void scan_pool(); // 遍历进程池
static int add_1_server(int *listensocket); // 增加进程
static int del_1_server(void); // 杀死进程
static void server_job(int pos,int **listensocket);   // 进程具体的工作函数
// 动态进程池
int main()
{
    setsignalwait();
    setsignalmy();
    sigset_t set,oldset;
    setsignalmy_set(&set,&oldset);
    

    // 建立数组的内存
    poolmemcreate();
    // 建立socket
    int listensocket = socketcreate();
    // 设置socket属性
    setsocketoptmy(&listensocket);
    // 绑定
    bindmy(&listensocket);
    // 监听
    listenmy(&listensocket);
    // 产生进程
    for(int i = 0; i < MINSPARESERVER; i++)
    {
        add_1_server(&listensocket);
    }
    // 当进程池内有进程的状态发生变化，就发一个信号通知我，我来看一下进程的状态
    while (1)
    {
        sigsuspend(&oldset);
        scan_pool();// 便利进程池

        if(idle_count > MAXSPARESERVER)
        {
            // 太多了杀掉一些进程
            for(int i = 0; i < (idle_count - MAXSPARESERVER); i++)
            {
                del_1_server();
            }
        }
        else if(idle_count < MINSPARESERVER)
        {
            // 太少了增加一些
            for(int slot = 0; slot < (MINSPARESERVER - idle_count); slot++)
            {
                add_1_server(&listensocket);
            }
        }

        // printf the pool state
        for(int i = 0; i < MAXCLIENTS; i++)
        {
            if(serverpool[i].pid == -1)
            {
                putchar('_');
            }
            else if(serverpool[i].state == STATE_IDEL)
            {
                putchar('.');
            }
            else
            {
                putchar('X');
            }
        }
        printf("\n");
    }
    close(listensocket);
    sigprocmask(SIG_SETMASK,&oldset,NULL); //恢复信号
    exit(0);
}

void setsignalwait(void)
{
    struct sigaction sa,oldsa;
    sa.sa_handler = SIG_IGN;                    // 设置信号行为--忽略
    sigemptyset(&sa.sa_mask);                   // 设置为空集，为什么没有人和我共用一个信号处理函数
    sa.sa_flags = SA_NOCLDWAIT;                 // 阻止收尸，自行收尸
    sigaction(SIGCHLD,&sa,&oldsa);              // 绑定信号行为
}

void setsignalmy(void)
{
    struct sigaction sa,oldsa;
    sa.sa_handler = usr2_handler;               // 设置信号行为
    sigemptyset(&sa.sa_mask);                   // 设置为空集，为什么没有人和我共用一个信号处理函数
    sa.sa_flags = 0;                            // 没有特殊要求
    sigaction(SIG_NOTIFY,&sa,&oldsa);           // 绑定信号行为
}

void setsignalmy_set(sigset_t *set,sigset_t *oldset)
{
    sigemptyset(set);
    sigaddset(set,SIG_NOTIFY);
    sigprocmask(SIG_BLOCK,set,oldset);// 不被信号打断
}

void usr2_handler(int s)
{
    return;
}

void poolmemcreate(void)
{
    /**
     * 内存映射函数mmap参数：
     * 1.可用的起始地址，写空 -- 让它自己找
     * 2.定义多大的一块内存
     * 3.映射过来的内存的权限 -- 读写
     * 4.共享还是独占, 第二个匿名映射，说明它不依赖于任何的文件
     * 5.所以文件描述符为-1,表示不依赖与任何文件
     * 6.偏移量，不依赖文件，所以为0
     */
    unsigned long size = sizeof(struct server_st)*MAXCLIENTS;
    serverpool = mmap(NULL,size,PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS,-1,0);
    if(serverpool == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }

    // 初始化
    for (int i = 0; i < MAXCLIENTS; i++)
    {
        serverpool[i].pid = -1;
    }
    
}

int socketcreate(void)
{
    int listen = socket(AF_INET,SOCK_STREAM,0);
    if(listen < 0)
    {
        perror("socket()");
        exit(1);
    }
    return listen;
}

void setsocketoptmy(int *s)
{
    int val = 1;
    int value = setsockopt(*s,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));
    if(value < 0)
    {
        perror("setsocketopt()");
        exit(1);
    }
}

void bindmy(int *listensocket)
{
    struct sockaddr_in localaddr;
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET,"0.0.0.0",&localaddr.sin_addr);
    int value = bind(*listensocket,(void*)&localaddr,sizeof(localaddr));
    if(value < 0)
    {
        perror("bind()");
        exit(1);
    }
}

void listenmy(int *listensocket)
{
    int value = listen(*listensocket,100);
    if(value < 0)
    {
        perror("listen()");
        exit(1);
    }
}

void scan_pool(void)
{
    for (int i = 0; i < MAXCLIENTS; i++)
    {
        if(serverpool[i].pid == -1)
        {
            continue;
        }
        if(kill(serverpool[i].pid,0))       // kill(pid,0) 是检测一个进程是否存在
        {
            serverpool[i].pid = -1;
            continue;
        }
        if(serverpool[i].state == STATE_IDEL)
        {
            idle_count++;
        }
        else if(serverpool[i].state == STATE_BUSY)
        {
            busy_count++;
        }
        else
        {
            fprintf(stderr,"Unknow state.\n");
            // _exit(1);
            abort();
        }
    }
    return;
}

int add_1_server(int *listensocket)
{
    if(idle_count + busy_count >= MAXCLIENTS)
    {
        return -1;
    }
    int slot;
    for (slot = 0; slot < MAXCLIENTS; slot++)
    {
        if(serverpool[slot].pid == -1)
        {
            break;
        }
    }
    serverpool[slot].state = STATE_IDEL;
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0)        // child
    {
        server_job(slot,&listensocket);
        exit(0);
    }
    else                // parent
    {
        serverpool[slot].pid = pid;
        idle_count++;
    }
    return 0;
}

int del_1_server(void)
{
    // 删除一个server端
    if(idle_count == 0)
    {
        return -1;
    }
    int i;
    for (i = 0; i < MAXCLIENTS; i++)
    {
        if(serverpool[i].pid != -1 && serverpool[i].state == STATE_IDEL)
        {
            kill(serverpool[i].pid,SIGTERM);
            serverpool[i].pid = -1;
            idle_count--;
            break;
        }
    }
    return 0;
}

void server_job(int pos, int **listensocket)
{
    pid_t ppid = getppid();
    struct sockaddr_in raddr;
    socklen_t raddr_len;
    int client_sd;
    time_t stamp;
    char linebuf[LINEBUFSIZE];
    char ipstr[IPSTRSIZE];
    while (1)
    {
        serverpool[pos].state = STATE_IDEL;
        kill(ppid,SIG_NOTIFY);     // 信号通知父进程，告诉现在的状态
        client_sd = accept(**listensocket,(void*)&raddr,&raddr_len);
        if(client_sd < 0)
        {
            if(errno != EINTR || errno != EAGAIN)
            {
                // 真错
                perror("accept()");
                exit(1);
            }
        }
        // 连接成功，把当前进程设置为忙碌，并通知父进程
        serverpool[pos].state = STATE_BUSY;
        kill(ppid,SIG_NOTIFY);
        inet_ntop(AF_INET,&raddr.sin_addr,ipstr,IPSTRSIZE);
        //printf("[%d]client:%s:%d\n",getpid(),ipstr,ntohs(raddr.sin_port));

        // 收消息
        stamp = time(NULL);
        int len = sprintf(linebuf,FMT_STAMP,stamp);
        // 发数据
        send(client_sd,linebuf,len,0);
        sleep(5);
        close(client_sd);
    }
}