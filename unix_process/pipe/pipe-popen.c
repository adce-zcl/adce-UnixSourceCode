/**
 * @file pipe-popen.c
 * 1.使用popen调用cat命令读取文件
 * 我们在用shell命令的时候，有一个管道通配符 |
 * 这个popen就是这个作用，启用一个shell命令，并预留管道通信
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FILEPATH "/home/adce/mycode/vscode/unix_process/pipe/file/pipeFile.txt"
#define PATHSIZE 1024
int main(int argc, char **argv)
{
    char command[PATHSIZE];
    char buf[1024];
    FILE *fp;
    sprintf(command, "cat %s", FILEPATH);
    fp = popen(command, "r");
    int readsize;
    while (fgets(buf, 1024, fp) != NULL)
    {
        printf(buf);
    }
    exit(0);
}