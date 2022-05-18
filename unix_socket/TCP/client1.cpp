// 只负责连接
#include <iostream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <cstdio>
using namespace std;
void hander(int p)
{
    exit(0);
}

void str_cli(int client_socket)
{
    char sendbuf[1024];
    string str;
    while (cin >> str)
    {
        memcpy(sendbuf, str.c_str(), strlen(str.c_str()));
        write(client_socket, sendbuf, strlen(sendbuf));
        int n;
        while ((n = read(client_socket, sendbuf, 1024)) > 0)
        {
            cout << sendbuf << endl;
        }
        cin.clear();
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr,"Using...\n");
        exit(0);
    }
    signal(SIGINT, hander);

    int port = atoi(argv[2]);
    string address = argv[1];
    
    // 新建套接字
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("socket");
        exit(-1);
    }

    // 建立服务端地址结构体
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &server_address.sin_addr.s_addr);

    // 请求连接
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("connect");
        close(client_socket);
        exit(-1);
    }
    cout << "connect success, IP: " << address << ", port: " << port << endl;

    str_cli(client_socket);

    exit(0);
}