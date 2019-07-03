#include "../Socket/Catnet.h"

static const char* request = "GET http://localhost/index.html HTTP/1.1\r\nConnection: keep-alive\r\n\r\nxxxxxxxxxxxxx";

int setnonblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

void addfd(int epoll_fd, int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLOUT | EPOLLET | EPOLLERR;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
	setnonblocking(fd);
}

int write_nbytes(int sockfd, const char* buffer, int len)
{
	int bytes_write = 0;
	printf("write out %d bytes to socket %d\n", len, sockfd);

	bytes_write = Cat_Sends(sockfd, buffer, len);
	if(bytes_write <= -0)
	{
		return -1;
	}
	return 0;
}

int read_once(int sockfd, char* buffer, int len)
{
	int bytes_read = 0;
	memset(buffer, '\0', len);
	bytes_read = Cat_Read(sockfd, buffer, len);
	if(bytes_read <= 0)
	{
		return -1;
	}
	printf("read in %d bytes from socket %d with content: %s\n", bytes_read, sockfd, buffer);

	return 0;
}

void start_conn(int epoll_fd, int num, const char* ip, int port)
{
	int ret = 0;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	socklen_t len = sizeof(struct sockaddr_in);
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	for(int i = 0; i < num; ++i)
	{
		sleep(1);
		int sockfd = Cat_Socket(PF_INET, SOCK_STREAM, 0);
		printf("create 1 sock\n");
		if(sockfd < 0)
		{
			continue;
		}

		Cat_Connect(sockfd, (struct sockaddr *)&address, (void *)&len);
		addfd(epoll_fd, sockfd);

	}

}

void close_conn(int epoll_fd, int sockfd)
{
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sockfd, 0);
	close(sockfd);
}

int main(int argc, char* argv[])
{
	//assert(argc == 4);
	if(argc < 4)
	{
		return -1;
	}
	int epoll_fd = epoll_create(100);
	start_conn(epoll_fd, atoi(argv[3]), argv[1], atoi(argv[2]));
	struct epoll_event events[10000];
	char buffer[2048];
	while(1)
	{
		int fds = epoll_wait(epoll_fd, events, 10000, 2000);
		for(int i = 0; i <fds; i++)
		{
			int sockfd = events[i].data.fd;
			if(events[i].events & EPOLLIN)
			{
				if(!read_once(sockfd, buffer, 2048))
				{
					close_conn(epoll_fd, sockfd);
				}
				struct epoll_event event;
				event.events = EPOLLOUT | EPOLLET | EPOLLERR;
				event.data.fd = sockfd;
				epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sockfd, &event);
			}
			else if(events[i].events & EPOLLOUT)
			{
				if(! write_nbytes(sockfd, request, strlen(request)) )
				{
					close_conn(epoll_fd, sockfd);
				}
				struct epoll_event event;
				event.events = EPOLLOUT | EPOLLET | EPOLLERR;
				event.data.fd = sockfd;
				epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sockfd, &event);

			}
			else if(events[i].events & EPOLLERR)
			{
				close_conn(epoll_fd, sockfd);
			}
		}
	}
}