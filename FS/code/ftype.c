#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
// 文件访问权限
// st_mode是一个16位的位图，由于表示文件类型
static int ftype(const char* filename)
{
    // linux里文件类型有七种，dcb-lsp
    struct stat statres;
    if(stat(filename,&statres) < 0)
    {
        perror("stat()");
        exit(1);
    }
    if(S_ISREG(statres.st_mode))
    {
        return '-';
    }
    else if (S_ISDIR(statres.st_mode))
    {
        return 'd';
    }
    else if(S_ISSOCK(statres.st_mode))
    {
        return 's';
    }
    // ...
    else
    {
        return '?';
    }
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Usage...\n");
        exit(1);
    }
    printf("%c\n",ftype(argv[1]));
    exit(0);
}