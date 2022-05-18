#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define LEFT 30000000
#define RIGHT 30000200
int main()
{
    int mark = 1;
    int count = 0;
    pid_t pid;
    for(int i = LEFT; i < RIGHT; i++)
    {
        pid = fork();
        if(pid < 0)
        {
            perror("fork()");
            exit(1);
        }
        if(pid == 0)
        {
            mark = 1;
            for(int j = 2; j < i/2; j++)
            {
                if(i%j == 0)
                {
                    mark = 0;
                    break;
                }
            }
            if(mark)
            {
                count++;
                printf("%d is primer.\n",i);
            }
            //sleep(1000);
            exit(0);
        }
    }
    //printf("%d\n",count);
    //sleep(1000);
    exit(0);
}