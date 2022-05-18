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
#include <string.h>
// 最宽松的权限
#define FILE_MODE (S_IRWXU | S_IRWXG | S_IRWXO)
#define FIFOCLIENT "/home/adce/mycode/vscode/unix_process/fifo/file/fifo.client"
#define FIFOSEVER "/home/adce/mycode/vscode/unix_process/fifo/file/fifo.server"
#define FILEPATH "/home/adce/mycode/vscode/unix_process/fifo/file/fifoFile.txt"
#define PATHSIZE 1024
void Client(int writefd, int readfd);
int main(int argc, char **argv)
{
    int readfd, writefd;
    writefd = open(FIFOCLIENT, O_WRONLY, 0);
    readfd = open(FIFOSEVER, O_RDONLY, 0);
    if (readfd < 0 || writefd < 0)
    {
        perror("open");
        exit(-1);
    }
    Client(writefd, readfd);
    unlink(FIFOCLIENT);
    unlink(FIFOSEVER);
    exit(0);
}
void Client(int writefd, int readfd)
{
    char pathbuf[PATHSIZE];
    strncpy(pathbuf, FILEPATH, PATHSIZE);
    size_t pathlen = strlen(pathbuf);   // 得到路径的真实长度
    ssize_t writesize = write(writefd, pathbuf, pathlen);
    if (writesize < pathlen)    // 如果写入的字节不够，就报错，默认是阻塞写，肯定会够，不够就是出错了
    {
        fprintf(stderr, "write size less than pathlen.\n");
        exit(-1);
    }
    // sleep(1);   // 等待服务端先把内容写入fifo，否则可能会读空
    // 循环读
    ssize_t readsize;
    char buf[1024];
    printf("client begin read:\n");
    while ((readsize = read(readfd, buf, 1024)) > 0)
    {
        write(STDOUT_FILENO, buf, readsize);    // 将读到的内容输出到标准输出
    }
    printf("client read over.\n");
    exit(0);
}