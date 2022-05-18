#include <setjmp.h>
#include <stdio.h>
#include <unistd.h>
int data = 3;
jmp_buf j_buf;
void func(int a, int b)
{
    a = 20;
    b = 10;
    data = 30;
    longjmp(j_buf, 1);
}
int main()
{
    volatile int b = 1;
    int a = 2;
    int res = setjmp(j_buf);
    if (res == 1)
    {
        printf("%d,%d,%d", data, a, b);
        return 0;
    }
    func(a, b);
}