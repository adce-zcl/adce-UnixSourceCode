/**
 * @file readdir.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021
 * 获取文件目录的方式之一，把文件目录当作是一个流
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#define PATH "/etc/"
int main(int argc ,char **argv)
{
    DIR *dp = NULL;
    struct dirent *current = NULL;

    dp = opendir(PATH);
    if(dp == NULL)
    {
        perror("opendir()");
        exit(1);
    }
    while ((current = readdir(dp)) != NULL)
    {
        puts(current->d_name);
    }
    
    closedir(dp);
    exit(0);
}