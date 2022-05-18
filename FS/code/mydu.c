#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include <string.h>
#define PATHSIZE 1024
//判断是否路径循环，. 和 .. 表示当前路径和上级路径，不能探索
static int path_noloop(const char * path)
{
    char *pos;
    //找出最右边的/
    pos = strrchr(path,'/');
    if(pos == NULL)
    {
        exit(1);
    }
    else
    {
        //如果这是本目录或者是上级目录
        if(strcmp(pos+1,".") == 0 || strcmp(pos+1, "..") == 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}


// long long 类型就 int64_t
static int64_t mydu(const char *path)
{
    //判断path是不是目录文件,应该用lstat，不要展开符号文件
    //找到是什么文件，就读什么文件

    //不跨递归的变量可以放在静态区
    static struct stat statres;
    static char nextPath[PATHSIZE];
    glob_t globres;
    int i = 0;
    int64_t sum=0;
    //用lstat获得path的属性信息
    if(lstat(path, &statres) < 0)
    {
        perror("lstat()");
        exit(1);
    }
    //如果不是目录
    if(!S_ISDIR(statres.st_mode))
    {
        //返回文件的字节数
        return statres.st_blocks;
    }
    //如果是目录
    //strncpy更安全，可以防止数组越界
    //在原本字符串的基础上追加/*  ，表示该目录下的所有文件
    strncpy(nextPath,path,PATHSIZE);
    strncat(nextPath,"/*",PATHSIZE);
    //解析路径中所有文件
    //glob函数会解析目录下的所有文件
    glob(nextPath,0,NULL,&globres);
    
    strncpy(nextPath,path,PATHSIZE);
    strncat(nextPath,"/.*",PATHSIZE);
    //解析文件中所有隐藏文件，并且是以追加的方式保存在globres里面
    glob(nextPath,GLOB_APPEND,NULL,&globres);
    
    sum = statres.st_blocks;
    for(i = 0; i < globres.gl_pathc; i++)
    {
        if(path_noloop(globres.gl_pathv[i]))
        {
            sum += mydu(globres.gl_pathv[i]);
        }
    }
    //sum += statres.st_blocks;
    globfree(&globres);
    return sum;
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }
    printf("%lld\n",mydu(argv[1])/2);
    exit(0);
}