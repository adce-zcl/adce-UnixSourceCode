#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <glob.h>


// 分割符
#define DELIMS " \t\n"

struct cmd_st
{
    glob_t globres;
};


/**
 * 两个字符串分割的函数
 * strtok
 * strsep
 * 
 */
static void prompt(void)
{
    printf("mysh-0.1$ ");
}

/**
 * @brief 
 * C语言的动态存储，argv是一方面，如果不能用argv，还有glob函数，非常类似argv
 * 如果glob函数也不能用，估计就只能自己动态申请内存了
 * 
 * @param line 
 */
static void parse(char *line, struct cmd_st *cmd)
{
    char *tok;
    int flag = 0;
    while (1)
    {
        tok = strsep(&line,DELIMS);
        if(tok == NULL)
        {
            break;
        }
        // 解析出来空串
        if(tok[0] == '\0')
        {
            continue;
        }
        // flag作为一个标记，只有第一次不追加，剩下的时候追加
        glob(tok,GLOB_NOCHECK | GLOB_APPEND*flag,NULL,&cmd->globres);
        flag = 1;
    }
}

int main()
{
    char *linebuf = NULL;
    size_t linebuf_size = 0;
    struct cmd_st cmd;
    while (1)
    {
        // 打印提示符
        prompt();
        if(getline(&linebuf,&linebuf_size,stdin)<0)
        {
            break;
        }

        // 解析
        parse(linebuf,&cmd);

        if(0) /*内部命令*/
        {
        }
        else if(1)
        {
            pid_t pid;
            pid = fork();
            if( pid < 0)
            {
                perror("fork()");
                exit(1);
            }
            if( pid == 0) // 子进程
            {
                execvp(cmd.globres.gl_pathv[0],cmd.globres.gl_pathv);
                perror("execvp");
                exit(1);
            }
            else // 父进程
            {
                wait(NULL);
            }
        }
    }
    exit(0);
}