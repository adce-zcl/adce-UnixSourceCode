/**
 * @file mydate.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-06
 * 
 * @copyright Copyright (c) 2022
 * 命令行参数分析
 * -y: year
 * -m:month
 * -d:day
 * -H:hour
 * -M:minute
 * -S:second
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#define TIMESTRSIZE 1024
#define FMTSTRSIZE 1024
int main(int argc, char** argv)
{
    time_t stamp;
    struct tm *tm;
    char timestr[TIMESTRSIZE];
    stamp = time(NULL);
    tm = localtime(&stamp);
    // 取出时间戳之后，应该使用getopt对命令行参数进行分析
    // 然后填充strftime的参数
    int c;
    char fmtstr[FMTSTRSIZE];
    extern char* optarg;
    extern int optind;
    fmtstr[0]='\0';
    FILE *fp;
    while(1)
    {
        c = getopt(argc,argv,"-H:MSy:md");
        if(c<0)
        {
            break;
        }
        switch (c)
        {
        // 非选项传参的处理
        case 1:
            // optind指向下一个参数
            fp = fopen(argv[optind-1],"w");
            if(fp == NULL)
            {
                perror("fopen()");
                // 输出由文件改为终端
                fp=stdout;
            }
            break;
        // 小时
        case 'H':
            if(strcmp(optarg,"12") == 0)
            {
                strncat(fmtstr,"%I(%P) ",FMTSTRSIZE);
            }
            else if(strcmp(optarg,"24") == 0)
            {
                strncat(fmtstr,"%H ",FMTSTRSIZE);
            }
            else
            {
                fprintf(stderr,"Invalid argument");
            }
            break;
        
        case 'M':
            strncat(fmtstr,"%M ",FMTSTRSIZE);
            break;

        case 'S':
            strncat(fmtstr,"%S ",FMTSTRSIZE);
            break;

        // 年份
        case 'y':
            if(strcmp(optarg,"2") == 0)
            {
                strncat(fmtstr,"%y ",FMTSTRSIZE);
            }
            else if(strcmp(optarg,"4") == 0)
            {
                strncat(fmtstr,"%Y ",FMTSTRSIZE);
            }
            else
            {
                fprintf(stderr,"Invalid\n");
            }
            break;

        case 'm':
            strncat(fmtstr,"%m ",FMTSTRSIZE);
            break;

        case 'd':
            strncat(fmtstr,"%d ",FMTSTRSIZE);
            break;

        default:
            break;
        }
    }
    strncat(fmtstr,"\n",FMTSTRSIZE);
    strftime(timestr,TIMESTRSIZE,fmtstr,tm);
    fputs(timestr,fp);
    if(fp != stdout)
    {
        fclose(fp);
    }

    exit(0);
}