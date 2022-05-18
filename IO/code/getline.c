#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    if(argc<2)
    {
        exit(1);
    }
    FILE *fp = fopen(argv[1],"r");
    if(fp == NULL)
    {
         perror("fopen:");
         exit(1);
    }
    char * linebuf = NULL;
    size_t linesize = 0;
    while(1)
    {
        if(getline(&linebuf,&linesize,fp)<0)
        {
            break;
        }
        printf("linebuf:%s",linebuf);
        printf("strlen(linebuf):%d\n",strlen(linebuf));
        printf("linesize:%d\n",linesize);
        printf("sizeof(linebuf):%d\n\n",sizeof(linebuf));
    }

    fclose(fp);
    return 0;
}