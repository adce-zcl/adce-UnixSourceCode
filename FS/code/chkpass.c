/**
 * 这个程序可以验证你输入的密码是否正确
 * 可以获得已有用户名的用户密码
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <shadow.h>
#include <string.h>
#include <crypt.h>
int main(int argc, char **argv)
{
    if(argc<2)
    {
        fprintf(stderr,"Usage..\n");
        exit(1);
    }
    // 要把终端的现实功能去掉，回显功能去调
    char *input_pass;
    input_pass = getpass("PassWord:");
    // puts(input_pasqs);

    // 在这一句的时候就应该校验用户权限，因为要访问shadow文件
    // 所以这个程序只能root用户使用
    struct spwd* shadowLine;
    shadowLine=getspnam(argv[1]);
    puts(shadowLine->sp_pwdp);

    // char *crypt(const char *phrase, const char *setting);
    // 第一个参数是口令原文
    // 第二个参数是杂串+$符号,使用已有密码相同的杂串计算
    char* crypt_pass;
    crypt_pass = crypt(input_pass,shadowLine->sp_pwdp);

    // 验证你登陆的口令跟我保存的口令是不是一样的
    if(strcmp(shadowLine->sp_pwdp,crypt_pass) == 0)
    {   
        puts("ok");
        
    }
    else
    {
        puts("false");
    }
    exit(0);
}