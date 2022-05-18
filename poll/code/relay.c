/**
 * @file relay.c
 * @author your name (you@domain.com)
 * @brief 中继 -- 有限状态机的编程方式
 * 这个程序因为调用了dev下面的设备，所以一定需要root用户
 * 使用poll
 * @version 0.1
 * @date 2022-02-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
// 两个设备
#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"

#undef BUFSIZ
#define BUFSIZ 1024

// 状态四选一
enum
{
    STATE_R = 1, // 读态
    STATE_W, // 写态
    STATE_Ex, // 异常处理态
    STATE_T // 终止态
};

struct fsm_st
{
    int state;
    int sfd;
    int dfd;
    // 缓冲区
    char buf[BUFSIZ];
    int len; // 读了多少字节
    int pos; // 标记写的位置
    char *errstr; // 出错的字符串
};


static void relay(int fd1, int fd2);
static void fsm_driver(struct fsm_st *fsm);
int main()
{
    int fd1,fd2;
    fd1 = open(TTY1,O_RDWR);
    if(fd1 < 0)
    {
        perror("open()");
        exit(1);
    }
    write(fd1,"tty1\n",5);
    fd2 = open(TTY2,O_RDWR | O_NONBLOCK);
    if(fd2 < 0)
    {
        perror("open()");
        exit(1);
    }
    write(fd2,"tty2\n",5);
    relay(fd1,fd2);

    close(fd1);
    close(fd2);
    exit(0);
}

void relay(int fd1, int fd2)
{
    int fd1_save = fcntl(fd1,F_GETFL);
    // 给文件描述符属性，追加非阻塞
    // 因为我们不能确保用户是用非阻塞的方式打开的
    fcntl(fd1,F_SETFL,fd1_save | O_NONBLOCK);
    int fd2_save = fcntl(fd2,F_GETFL);
    fcntl(fd2,F_SETFL,fd2_save | O_NONBLOCK);
    struct fsm_st fsm12,fsm21;
    fsm12.state = STATE_R;
    fsm12.sfd = fd1;
    fsm12.dfd = fd2;
    fsm21.state = STATE_R;
    fsm21.sfd = fd2;
    fsm21.dfd = fd1;

    // 设置集合
    struct pollfd pfd[2];
    // 布置监视任务
    pfd[0].fd = fd1;
    pfd[1].fd = fd2;
  
    // 这就是忙等的关键,使用select可以避免忙等
    while (fsm12.state != STATE_T || fsm21.state != STATE_T)
    {
        // 布置监视任务
        pfd[0].events = 0; //监视的事件清零
        pfd[1].events = 0;
        if(fsm12.state == STATE_R)
        {
            pfd[0].events |= POLLIN;
        }
        if(fsm12.state == STATE_W)
        {
           pfd[1].events |= POLLOUT;
        }
        if(fsm21.state == STATE_R)
        {
            pfd[1].events |= POLLIN;
        }
        if(fsm21.state == STATE_W)
        {
            pfd[0].events |= POLLOUT;
        }

        // 监视
        // rset和wset是可以被修改的，所以不能重复使用
        while(poll(pfd,2,-1) < 0) //timeout设置为-1阻塞，直到发生感兴趣的行为为止
        {
            if(errno == EINTR)
            {
                continue;
            }
            perror("poll()");
            exit(1);
        }

        // 查看监视结果
        // 根据监视结果，有条件的推动状态机
        if(pfd[0].revents & POLLIN || pfd[1].revents & POLLOUT)
        {
            fsm_driver(&fsm12);
        }
        if(pfd[1].revents & POLLIN || pfd[0].revents & POLLOUT)
        {
            fsm_driver(&fsm21);
        }
    }
    

    // 恢复原文件的属性
    fcntl(fd2,F_SETFL,fd2_save);
    fcntl(fd1,F_SETFL,fd1_save);
}

void fsm_driver(struct fsm_st *fsm)
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