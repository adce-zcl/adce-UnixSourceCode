#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE 1024
int main(int argc, char **argv)
{
    if(argc<3)
    {
        fprintf(stderr,"Usage\n");
        exit(1);
    } 
    int fd1,fd2;
    fd1 = open(argv[1],O_RDONLY);
    if(fd1<0)
    {
        perror("open()");
        exit(1);
    }
    fd2 = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,00600);
    if(fd2<0)
    {
        perror("open()");
        close(fd1);
        exit(1);
    }
    printf("fd1:%d,fd2:%d\n",fd1,fd2);
    char buf[BUFSIZE];
    int len;
    int ret;
    while(1)
    {
        len = read(fd1,buf,BUFSIZE);
        if(len<0)
        {
            perror("read()");
            break;
        }
         if(len == 0)
        {
            break;
        }
        //确保写入的数量和读出来的数量一样
        int pos = 0;
        while(len>0)
        {
            ret = write(fd2,buf+pos,len);
            if(ret < 0)
            {
                perror("write()");
                exit(1);
             }
             pos+=ret;
             len-=ret;
        }
    }

    close(fd1);
    close(fd2);
     return 0;
}