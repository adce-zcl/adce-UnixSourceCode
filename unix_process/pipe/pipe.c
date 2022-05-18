/**
 * @file pipe.c
 * 1.客户端传送给服务端一个路径名字
 * 2.服务端读取文件内容通过ipc传送给客户端
 * 3.客户端将读取的内容显示到标准输出
 * 
 * 管道本身是半双工，fork之后是可以全双工，但是只有一个管道缓冲区，根本没法进行通信
 * 全双工的通信还是像下面这样，用两个管道，用两个缓冲区实现
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#define FILEPATH "/home/adce/code_unixnetwork/unix_process/pipe/file/pipeFile.txt"
#define PATHSIZE 1024
void Client(int writefd, int readfd);
void Server(int writefd, int readfd);
int main(int argc, char **argv)
{
    int pipe_client[2]; // 客户端写的管道
    int pipe_server[2]; // 服务端写的管道
    if (pipe(pipe_client) < 0 || pipe(pipe_server) < 0)
    {
        perror("pipe");
        exit(-1);
    }
    // 让子进程运行服务端程序，父进程运行客户端程序
    pid_t server_pid;
    if ((server_pid = fork()) < 0)
    {
        perror("fork");
        exit(-1);
    }
    if (server_pid == 0)    // 子进程运行服务端程序
    {
        close(pipe_client[1]);  // 关掉客户端的写通道
        close(pipe_server[0]);  // 关掉服务端的读通道
        Server(pipe_server[1], pipe_client[0]); // 执行服务端程序
        exit(0);    // 执行完就结束
    }
    // 父进程运行客户端程序
    close(pipe_client[0]);  // 关掉客户端的读端
    close(pipe_server[1]);  // 关掉服务端的写端
    Client(pipe_client[1], pipe_server[0]);
    waitpid(server_pid, NULL, 0);   // 收尸子进程
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