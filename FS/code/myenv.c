#include <stdio.h>
#include <stdlib.h>
int main()
{
    extern char** environ;
    int i;
    for(i = 0; environ[i] != NULL; i++)
    {
        puts(environ[i]);
    }
    exit(0);
}