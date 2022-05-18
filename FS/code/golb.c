#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>
#define PATH "/etc/*"
int errFunc_(const char *errpath, int errno)
{
    puts(errpath);
    fprintf(stderr, "ERROR MSG:%s\n", strerror(errno));
    return 0;
}
int main(int argc, char **argv)
{
    glob_t globRes;

    // 第二个参数flag和位图有关，0表示不需要什么标志
    int err = glob(PATH, 0, errFunc_, &globRes);
    if(err)
    {
        printf("Error code = %d\n",err);
        exit(1);
    }
    for(int i = 0; i < globRes.gl_pathc; i++)
    {
        puts(globRes.gl_pathv[i]);
    }

    globfree(&globRes);

    exit(0);
}