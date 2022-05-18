#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>

//初始化服务器的监听端口
int initserver(int port);

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		std::cout << "usage: ./selectserver port\n";
		return -1;
	}

	//初始化服务端用于监听的socket
	int listensock = initserver(atoi(argv[1]));
	std::cout << "listensock=" << listensock << std::endl;

	if(listensock < 0)
	{
		std::cout << "initserver() faile\n";
		return -1;
	}

	fd_set readfdset; //读事件的集合，包括监听socket和客户端连接上来的socket
	int maxfd; //readfdset中socket的最大值

	//初始化结构体，把listensock添加到集合中
	FD_ZERO(&readfdset);
	FD_SET(listensock, &readfdset);
	maxfd = listensock;

	while(1)
	{
		//调用select函数时，会改变socket结合的内容，所以要把socket集合保存下来
		//传一个临时的集合给select
		fd_set tmpfdset = readfdset;

		int infds = select(maxfd+1, &tmpfdset, NULL, NULL, NULL);
		// std::cout << "select infds = " << infds << std::endl;

		//返回失败
		if(infds < 0)
		{
			std::cout << "select() failed\n";
			break;
		}

		//超时，在本程序中，select函数最后一个参数为空，不存在超市的情况，但以下代码还是留着
		if (infds == 0)
		{
			std::cout << "select() timeout\n";
		}

		//检查到有事件发生的socket，包括监听和客户端连接的socket
		//这里时客户端的socket事件，每次都要遍历整个集合，因为可能有多个socket有事件
		for (int eventfd = 0; eventfd <= maxfd; eventfd++)
		{
			if (FD_ISSET(eventfd, &tmpfdset) <= 0)
			{
				continue;
			}

			if (eventfd == listensock)
			{
				//如果发生事件的时listensock，表示有新的客户端连接上来
				struct sockaddr_in client;
				socklen_t len = sizeof(client);
				int clientsock = accept(listensock, (struct sockaddr*)&client, &len);
				
				if (clientsock < 0)
				{
					std::cout << "accept() failed\n";
					continue;
				}
				std::cout << "client(socket=" << clientsock << ") connected ok \n";

				//把新的客户端socket加入集合
				FD_SET(clientsock, &readfdset);

				if (maxfd < clientsock)
				{
					maxfd = clientsock;
				}
				continue;

			}
			else
			{
				//客户端有数据过来或客户端的socket连接被断开
				char buffer[1024];
				memset(buffer, 0, 1024);

				//读取客户端的数据
				ssize_t isize = read(eventfd, buffer, sizeof(buffer));

				//发生了错误或socket被对方关闭
				if (isize <= 0)
				{
					std::cout << "client(eventfd = " << eventfd << ") disconnected\n";
					close(eventfd);
					//从集合中移除客户端的socket
					FD_CLR(eventfd, &readfdset);

					//重新计算maxfd的值，注意，只有当eventfd == maxfd时才需要计算
					if (eventfd == maxfd)
					{
						for (int i = maxfd; i > 0; i--)
						{
							if (FD_ISSET(i, &readfdset))
							{
								maxfd = i;
								break;
							}
						}

						std::cout << "maxfd = " << maxfd << std::endl;
					}

					continue;
				}

				std::cout << "eventfd = " << eventfd << ",size = " << isize << ",buffer = " << buffer << std::endl;

				//把收到的报文发送给客户端
				write(eventfd, buffer, strlen(buffer));
			}
		}
	}

	return 0;
}

//初始化监听sock的代码
int initserver(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		std::cout << "socker() failed\n";
		return -1;
	}

	//Linux如下
	int opt = 1;
	unsigned int len = sizeof(opt);
	//这两句啥意思，没看懂
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, len);
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &opt, len);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		std::cout << "bind() failed\n";
	}
	if (listen(sock, 5) != 0)
	{
		std::cout << "listen() failed\n";
		return -1;
	}
	return sock;
}
















