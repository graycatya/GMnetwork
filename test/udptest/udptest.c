#include "Catnet.h"
#include <pthread.h>
#include <semaphore.h>

int main(int argc, char *argv[])
{
	if(argc <= 2)
	{
		return -1;
	}
	int port = atoi(argv[2]);

	int sockfd = Cat_UdpScoket(argv[1], port, CAT_IPV4);

	int ret = 0;	

	struct sockaddr_in server;
	int server_len = sizeof(server);

	char buf[1024];
	memset(buf, '\0', 1024);
	ret = Cat_UdpRead(sockfd, buf, 1024-1, (struct sockaddr*)&server, (void *)&server_len);
	printf("buf %s\n", buf);
	ret = Cat_UdpSend(sockfd, (const void*)buf, ret, (const struct sockaddr*)&server, (void *)&server_len);
	//Sleep(1);
	Cat_Close(sockfd);
	return 0;
}
