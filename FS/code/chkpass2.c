#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <crypt.h>
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        perror("Usage..");
        exit(0);
    }
    char *user_name = argv[1];
    struct passwd *pw = getpwnam(user_name);

    char *pswd = getpass("Passwd:");
    // 不能这样，只能通过shadow函数来进行密码校验
    char *pswd_cry = crypt(pswd, pw->pw_passwd);
    if (strcmp(pswd_cry, pw->pw_passwd) == 0)
    {
        puts("OK");
    }
    else
    {
        puts("false");
    }
    exit(0);
}