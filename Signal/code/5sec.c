/**
 * @file 5sec.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-11
 * 
 * @copyright Copyright (c) 2022
 * 
 * 使用time计时
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main()
{
    long long int count = 0;
    time_t end;
    end = time(NULL)+5;

    while(time(NULL) <= end)
    {
        count++;
    }
    printf("%lld\n",count);
    exit(0);
}