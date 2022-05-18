/**
 * @file ansi_vt.c
 * @author your name (you@domain.com)
 * @brief VT控制码
 * @version 0.1
 * @date 2022-02-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // 打印hello！
    printf("\033[2J \033[10;10H \033[4m \033[31;4mhello!\n\033[0m");
    
    exit(0);
}