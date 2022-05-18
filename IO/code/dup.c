#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define FNAME "dup.txt"

int main()
{
    int fd;
    //关闭标准输入流
    
    fd = open(FNAME,O_WRONLY |O_APPEND,0600);
    if(fd < 0)
    {
        perror("open");
        exit(1);
    }
    // close(1); 
    // int fd2 = dup(fd);
    // dup2 = 原子操作 colse 1 + dup fd
    dup2(fd,1);
    close(fd); 
    puts("fd3 write");
    return 0;
}