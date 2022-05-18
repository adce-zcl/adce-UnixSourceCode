#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    char *str = getlogin();
    puts(str);
    exit(0);
}