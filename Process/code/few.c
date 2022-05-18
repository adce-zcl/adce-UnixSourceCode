#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    puts("Begin.");

    fflush(NULL);
    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    if(pid == 0)
    {
        // 子进程
        execl("/usr/bin/date","+%s",NULL);
        perror("...");
        exit(1);
    }
    else
    {
        wait(NULL);
    }
    puts("End.");
    exit(0);
}