/**
 * @file fifo-server.c
 * 使用命名管道通信
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
// 最宽松的权限
#define FILE_MODE (S_IRWXU | S_IRWXG | S_IRWXO)
#define FIFOSEVER "/home/adce/mycode/vscode/unix_process/fifo/file/fifo.server"
#define FIFOCLIENT "/home/adce/mycode/vscode/unix_process/fifo/file/fifo.client"
#define PATHSIZE 1024
void Server(int writefd, int readfd);
int main(int argc, char **argv)
{
    if ((mkfifo(FIFOSEVER, FILE_MODE) < 0) && (errno != EEXIST))
    {
        perror("mkfifo");
        exit(-1);
    }
    if ((mkfifo(FIFOCLIENT, FILE_MODE) < 0) && (errno != EEXIST))
    {
        perror("mkfifo");
        exit(-1);
    }
    int readfd, writefd;
    readfd = open(FIFOCLIENT, O_RDONLY, 0);
    writefd = open(FIFOSEVER, O_WRONLY, 0);
    if (readfd < 0 || writefd < 0)
    {
        perror("open");
        exit(-1);
    }
    Server(writefd, readfd);
    exit(0);
}

void Server(int writefd, int readfd)
{
    char buf[PATHSIZE];
    ssize_t readsize;
    if((readsize = read(readfd, buf, PATHSIZE)) < 0) // 先把路径读出来
    {
        // 读出错
        perror("read");
        exit(-1);
    }
    int filefd;
    filefd = open(buf, O_RDONLY);
    if (filefd < 0)
    {
        fprintf(stderr, "open() failed.");
        exit(-1);
    }
    while ((readsize = read(filefd, buf, PATHSIZE)) > 0)
    {
        write(writefd, buf, readsize);
    }
    write(writefd, "send over!\n", 12);
    exit(0);
}