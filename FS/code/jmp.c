#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
// __FUNCTION__ 可以打印本函数的名字
// 因为要跨函数跳转，所以要声明为全局变量，要习惯在声明全局变量的时候使用static修饰
// static修饰可以避免作用域污染，因为你要时刻记住你在写一个模块。
static jmp_buf save; 
static void d(void)
{
    printf("%s():Begin.\n",__FUNCTION__);
    // 跳转
    printf("%s():Jump now !\n",__FUNCTION__);
    longjmp(save,0);
    printf("%s():End.\n",__FUNCTION__);
}

static void c(void)
{
    printf("%s():Begin.\n",__FUNCTION__);
    printf("%s():Call d().\n",__FUNCTION__);
    d();
    printf("%s():d() returned.\n",__FUNCTION__);
    printf("%s():End.\n",__FUNCTION__);
}

static void b(void)
{
    printf("%s():Begin.\n",__FUNCTION__);
    
    printf("%s():Call c().\n",__FUNCTION__);
    c();
    printf("%s():c() returned.\n",__FUNCTION__);
    printf("%s():End.\n",__FUNCTION__);
}

static void a(void)
{
    
    printf("%s():Begin.\n",__FUNCTION__);
    int ret = setjmp(save);
    if(ret == 0)
    {
        // 设置跳转点
        printf("%s():Call b().\n",__FUNCTION__);
        b();
        printf("%s():b() returned.\n",__FUNCTION__);
    }
    else
    {
        // 从别处跳回来的
        printf("%s():Jumped back here with code %d\n",__FUNCTION__,ret);
    }
    
    printf("%s():End.\n",__FUNCTION__);
}



int main()
{
    printf("%s():Begin.\n",__FUNCTION__);
    printf("%s():Call a().\n",__FUNCTION__);
    a();
    printf("%s():a() returned.\n",__FUNCTION__);
    printf("%s():End.\n",__FUNCTION__);
    exit(0);
}