/**
 * @file set_tattr.c
 * @author your name (you@domain.com)
 * @brief 设置键盘回显
 * @version 0.1
 * @date 2022-02-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

int main()
{
    char ch;
    struct termios new,old;                 // 保存某一个文件的属性
    tcgetattr(0,&new);               // 先将标准输入的属性保存下载，再在原有属性的基础上更改
    old = new;
    new.c_lflag = new.c_lflag & (~(ICANON | ECHO));     // 去掉回显,去掉换行结束标记
    new.c_cc[VMIN] = 1;                               // 一次接收几个字符
    new.c_cc[VTIME] = 0;                              // 有没有时间设置
    tcsetattr(0,TCSANOW,&new);
    while (1)
    {
        ch = getchar();
        if(ch == 'q')
        {
            break;
        }
        printf("%c ",ch);
        
    }
    tcsetattr(0,TCSANOW,&old);              // 还原
    exit(0);
}