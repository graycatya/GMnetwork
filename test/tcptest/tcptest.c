#include "Catnet.h"

int main(int argc, char *argv[])
{
	if(argc <= 2)
	{
		return -1;
	}
	int port = atoi(argv[2]);

	int sockfd = Cat_TcpScoket(argv[1], port, CAT_IPV4);

	int ret = Cat_Listen(sockfd, 5);

	struct sockaddr_in server;

	#ifdef _WINDOWSSYS_ 
		int server_len = sizeof(server);
	#else
		socklen_t server_len = sizeof(server);
	#endif

	int connfd = Cat_Accept(sockfd, (struct sockaddr*)&server, (void *)&server_len, 0);

	char buf[1024];
	memset(buf, '\0', 1024);
	ret = Cat_Read(connfd, buf, 1024-1);

	ret = Cat_Send(connfd, buf, ret);
	Sleep(1);
	Cat_Close(sockfd);

	return 0;
}
