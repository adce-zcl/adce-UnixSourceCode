#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"./username ... \n");
        exit(1);
    }
    uid_t uid = atoi(argv[1]);
    struct passwd *PD;
    PD = getpwuid(uid);
    puts(PD->pw_name);
    exit(0);
}