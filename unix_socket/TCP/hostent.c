#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
extern int h_errno;
int main(int argc, char **argv)
{
    char *ptr;
    struct hostent *hptr;
    // 循环处理argv
    while (--argc > 0)
    {
        ptr = *(++argv);

        // 使用gethostbyname()通过名字解析IP地址
        hptr = gethostbyname(ptr);
        if (hptr == NULL)   // 这个主机名解析出错，继续解析下一个
        {
            fprintf(stderr, "gethostbyname error by host:%s:%s\n", ptr, hstrerror(h_errno));
            continue;
        }
        
        // 正式主机名
        printf("official hostname:%s\n", hptr->h_name);

        // 解析别名
        char **pptr;
        pptr = hptr->h_aliases;
        for ( pptr; *pptr != NULL; pptr++)
        {
            printf("talias:%s\n", *pptr);
        }

        // 解析IP地址,只解析IPV4
        if (hptr->h_addrtype == AF_INET)
        {
            pptr = hptr->h_addr_list;
            for ( pptr; *pptr != NULL; pptr++)
            {
                char str[INET_ADDRSTRLEN];
                inet_ntop(hptr->h_addrtype, *pptr, str, INET_ADDRSTRLEN);
                printf("address:%s\n", str);
            }
        }
    }
    exit(0);
}