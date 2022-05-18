// 数据结构要隐藏在.c文件当中
#include "mytbf.h"
#include <sys/time.h>
#include <signal.h>
struct mytbf_st
{
    int cps;
    int burst;
    int token;
    int pos;
};

// static 数组，一个是不会被环境污染，另一个就是会初始化为全0
static struct mytbf_st* job[MYTBF_MAX];
// 全局标志变量
static int inited = 0;
// signal信号量功能保存


static struct sigaction alrm_sa_save;

static int get_free_pos(void)
{
    for (int i = 0; i < MYTBF_MAX; i++)
    {
        if(job[i] == NULL)
        {
            return i; // 返回空位
        }
    }
    return -1; // 没有空位
}

static int min(int a, int b)
{
    if(a < b)
    {
        return a;
    }
    return b;
}

// 修改信号响应函数，只响应kernal过来的信号
static void alrm_action(int s, siginfo_t *infop, void *unused)
{
    // alarm(1);
    if(infop->si_code != SI_KERNEL)
    {
        return;
    }
    for (int i = 0; i < MYTBF_MAX; i++)
    {
        if(job[i] != NULL)
        {
            job[i]->token += job[i]->cps;
            if(job[i]->token > job[i]->burst)
            {
                job[i]->token = job[i]->burst;
            }
        }
    }
}


static void module_unload(void);
static void module_load(void)
{
    //alrm_hander_save = signal(SIGALRM,alrm_handler);
    //alarm(1);
    
    struct sigaction sa;
    sa.sa_sigaction = alrm_action; //三个参数的信号处理函数
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO; // 表示我给的信号处理函数是三个参数的
    sigaction(SIGALRM,&sa,&alrm_sa_save); //这个要判断失败，这里就不判断了
    
    struct itimerval itv;
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL,&itv,NULL); //需要恢复的话要保存之前的状态
    
    atexit(module_unload);
}

// 这个模块只能在库函数完全结束，或者程序异常终止的时候使用
// 绝对不能在destory函数里使用，因为它只是销毁一个令牌桶
// 而这个函数会使得整个库功能不复存在，所以这里使用钩子函数,
// 在load函数里加载钩子函数
static void module_unload(void)
{
    // 作为一个单独的模块，进入模块和出去模块，程序的状态应该是一样的
    // 所以，为了防止信号对后续的程序有影响，要卸载模块
    //signal(SIGALRM,alrm_hander_save);
    //alarm(0);

    sigaction(SIGALRM,&alrm_sa_save,NULL);

    struct itimerval itv;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL,&itv,NULL); //需要恢复的话要保存之前的状态

    for (int i = 0; i < MYTBF_MAX; i++)
    {
        free(job[i]);
    }
}

// 初始化，申请结构体空间，并将结构体放入数组
// cps表示申请的数据的传输速率，burst表示令牌桶的上限
mytbf_t *mytbf_init(int cps,int burst)
{
    int pos = get_free_pos(); // 寻找数组的空位
    if(pos < 0)
    {
        // 没有空位
        return NULL;
    }
    if(!inited)
    {
        module_load();
        inited = 1;
    }
    struct mytbf_st *me;
    me = malloc(sizeof(*me));
    if(me == NULL)
    {
        return NULL;
    }
    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    me->pos = pos;
    job[pos] = me;
    return me;
}

// 取出令牌，从ptr里面取出size个token
int mytbf_fetchtoken(mytbf_t *ptr ,int size)
{
    struct mytbf_st *me = ptr;
    if(size <= 0)
    {
        return -EINVAL; //参数非法，自己设置errno数值
    }
    while (me->token <= 0)
    {
        pause();
    }
    int n = min(me->token,size);
    // 用完要减去
    me->token -= n;
    return n;
}

// 归还token
int mytbf_returntoken(mytbf_t *ptr, int size)
{
    if(size <= 0)
    {
        return -EINVAL;
    }
    struct mytbf_st *me = ptr;
    me->token += size;
    if(me->token > me->burst)
    {
        me->token = me->burst;
    }
    return size;
}

int mytbf_destory(mytbf_t *ptr)
{
    // 找到数组的对应位置
    struct mytbf_st *me = ptr;
    job[me->pos] = NULL;
    free(ptr);
    return 0;
}
