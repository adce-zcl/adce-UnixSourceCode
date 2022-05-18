/**
 * @file mmap.c
 * @author your name (you@domain.com)
 * @brief 随意指定一个文件，描述这个文件有多少个字符
 * @version 0.1
 * @date 2022-02-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
 
int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Usage..\n");
        exit(1);
    }

    // 得到文件描述符
    int fd = open(argv[1],O_RDONLY);
    if(fd < 0)
    {
        perror("open()");
        exit(1);
    }

    // 获取文件属性
    struct stat st;
    if(fstat(fd,&st) < 0)
    {
        perror("fstat()");
        exit(1);
    }

    char *str = mmap(NULL,st.st_size,PROT_READ,MAP_SHARED,fd,0);
    if(str == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }
    close(fd);

    long int count = 0;
    for (int i = 0; i < st.st_size; i++)
    {
        if(str[i] == 'a') // 找出这个文件里面有多少个a
        {
            count++;
        }
    }
    printf("size:%ld\n",count);
    
    munmap(str,st.st_size);
    exit(0);
}