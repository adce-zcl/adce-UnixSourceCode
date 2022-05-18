#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
extern char **environ;

/*
 * date + %s
 */

int main()
{
    puts("Begin.");

    // 这个程序从这一条语句变成date进程，后面的不再执行
    // 在exec之前一定要刷新该刷新的流，因为有进程切换
    fflush(NULL);
    execl("/usr/bin/date","date",NULL);
    perror("execl()");
    exit(1);

    puts("End.");
    exit(0);
}