#include "Catnet.h"

void Cat_Errexit(const char *s)
{
	perror(s);
	exit(-1);
}

int Cat_Socket(int family, int sockettype, int protocol)
{
	int sock;
	if((sock = socket(family, sockettype, protocol)) < 0)
	{
		Cat_Errexit("Cat_Socket error");
	}
	return sock;
}

int Cat_Bind(int fd, struct sockaddr *addr, void* addrlen)
{
	#ifdef _WINDOWSSYS_
	int* addlen = (int*)addrlen;
	#else
	strlen_t* addlen = (strlen_t*)addrlen;
	#endif

	if(addr == NULL)
	{
		Cat_Errexit("Cat Bind addr error");

	}
	int ret;
	if((ret = bind(fd, addr, *addlen)) < 0)
	{
		Cat_Errexit("Cat Bind error");
	}
	return ret;
}

int Cat_Listen(int fd, int backlog)
{
	int ret;
	if((ret = listen(fd, backlog)) < 0)
	{
		Cat_Errexit("Cat_Listen error");
	}
	return ret;
}


int Cat_Accept(int fd, struct sockaddr *addr, void *addrlen, int connects)
{
	#ifdef _WINDOWSSYS_
	int* addlen = (int*)addrlen;
	#else
	strlen_t* addlen = (strlen_t*)addrlen;
	#endif
	int ret, Connectiontimes = 0;
connect:
	if((ret = accept(fd, addr, addlen)) < 0)
	{
		if( (errno == ECONNABORTED) || (errno == EINTR) )
		{
			if( Connectiontimes < connects )
			{
				Connectiontimes++;
				goto connect;
			}
			else
			{
				
			}
		}
		else
		{
			Cat_Errexit("Cat_Accept error");
		}
	}
	return ret;
}

int Cat_Connect(int fd, const struct sockaddr *addr, void* addrlen)
{
	#ifdef _WINDOWSSYS_
	int* addlen = (int*)addrlen;
	#else
	strlen_t* addlen = (strlen_t*)addrlen;
	#endif
	int ret;
	if((ret = connect(fd, addr, *addlen)) < 0)
	{
		Cat_Errexit("Cat_Connect error");
	}
	return ret;
}

int Cat_Close(int fd)
{
	int ret;
	#ifdef _WINDOWSSYS_
		if((ret = closesocket(fd)) == -1)
		{
			Cat_Errexit("Cat_Close error");
		}
		WSACleanup();//结束调用
	#else
		if((ret = close(fd)) == -1)
		{
			Cat_Errexit("Cat_Close error");
		}
	#endif
	return ret;
}

ssize_t Cat_Read(int fd, void *buf, size_t len)
{
	#ifdef _WINDOWSSYS_
		char* bufs = (char*)buf;
	#else
		void* bufs = buf;
	#endif
	ssize_t ret;

again:
	if( (ret = recv(fd, bufs, len, 0)) < 0)
	{
		if(errno == EINTR)
		{
			goto again;
		}
		else
		{
			return -1;
		}
	}
	return ret;
	
}

ssize_t Cat_Send(int fd, const void *buf, size_t len)
{
	#ifdef _WINDOWSSYS_
		const char* bufs = (const char*)buf;
	#else
		void* bufs = buf;
	#endif
	ssize_t ret;

again:
	if( (ret = send(fd, bufs, len, 0)) <= 0)
	{
		if(errno == EINTR)
		{

			goto again;
		}
		else
		{
			return -1;
		}
	}
	return ret;
}

ssize_t Cat_Reads(int fd, void *buf, size_t len)
{
	size_t nleft;	//剩余未读的字节数
	ssize_t nread;	//实际读取的字节数
	char	*ptr;

	ptr = (char *)buf;
	nleft = len;

	while(nleft > 0)
	{
		printf("ptr %s\n", ptr);
		if( (nread = recv(fd, ptr, nleft, 0)) < 0)
		{
			if(errno == EINTR)
			{
				nread = 0;
			}
			else
			{
				return -1;
			}
		}
		else if(nread == 0)
		{
			break;
		}

		nleft -= nread;
	}

	return len - nleft;
}

ssize_t Cat_Sends(int fd, const void *buf, size_t len)
{
	size_t nleft;	//剩余未发送的字节数
	ssize_t nsend;	//实际发送取的字节数
	const char	*ptr;

	ptr = (char *)buf;
	nleft = len;

	while(nleft > 0)
	{
		if( (nsend = send(fd, ptr, nleft, 0)) <= 0)
		{
			if(nsend < 0 && errno == EINTR)
			{
				nsend = 0;
			}
			else
			{
				return -1;
			}
		}
		else if(nsend == 0)
		{
			break;
		}

		nleft -= nsend;
		ptr += nsend;
	}
	return len;
}


ssize_t Cat_UdpRead(int fd, void *buf, size_t len, struct sockaddr* src_addr, void* addrlen)
{
	#ifdef _WINDOWSSYS_
	int* addlen = (int*)addrlen;
	#else
	strlen_t* addlen = (strlen_t*)addrlen;
	#endif
	ssize_t ret;

again:

	if( (ret = recvfrom(fd, buf, len, 0, src_addr, addlen)) < 0)
	{
		if(errno == EINTR)
		{
			goto again;
		}
		else
		{
			return -1;
		}
	}
	return ret;

}

ssize_t Cat_UdpSend(int fd, const void *buf, size_t len, const struct sockaddr* dest_addr, void* addrlen)
{
	#ifdef _WINDOWSSYS_
	int* addlen = (int*)addrlen;
	#else
	strlen_t* addlen = (strlen_t*)addrlen;
	#endif

	ssize_t ret;

again:
	if( (ret = sendto(fd, buf, len, 0, dest_addr, *addlen)) <= 0)
	{
		if(errno == EINTR)
		{
			goto again;
		}
		else
		{
			return -1;
		}
	}
	return ret;
}

ssize_t Cat_UdpReads(int fd, void *buf, size_t len, struct sockaddr* src_addr, void* addrlen)
{
	#ifdef _WINDOWSSYS_
	int* addlen = (int*)addrlen;
	#else
	strlen_t* addlen = (strlen_t*)addrlen;
	#endif
	size_t nleft;	//剩余未读的字节数
	ssize_t nread;	//实际读取的字节数
	char	*ptr;

	ptr = buf;
	nleft = len;

	while(nleft > 0)
	{
		if( (nread = recvfrom(fd, ptr, nleft, 0, src_addr, addlen)) < 0)
		{
			if(errno == EINTR)
			{
				nread = 0;
			}
			else
			{
				return -1;
			}
		}
		else if(nread == 0)
		{
			break;
		}

		nleft -= nread;
	}

	return len - nleft;
}

ssize_t Cat_UdpSends(int fd, const void *buf, size_t len, const struct sockaddr* dest_addr, void* addrlen)
{
	#ifdef _WINDOWSSYS_
	int* addlen = (int*)addrlen;
	#else
	strlen_t* addlen = (strlen_t*)addrlen;
	#endif
	size_t nleft;	//剩余未发送的字节数
	ssize_t nsend;	//实际发送取的字节数
	const char	*ptr;

	ptr = buf;
	nleft = len;

	while(nleft > 0)
	{
		if( (nsend = sendto(fd, ptr, nleft, 0, dest_addr, *addlen)) <= 0)
		{
			if(nsend < 0 && errno == EINTR)
			{
				nsend = 0;
			}
			else
			{
				return -1;
			}
		}
		else if(nsend == 0)
		{
			break;
		}

		nleft -= nsend;
		ptr += nsend;
	}
	return len;
}

int Cat_TcpScoket(const char* IP, short Port, int Ip_family)
{
	#ifdef _WINDOWSSYS_
		//初始化WSA
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0)
		{
			return 0;
		}
	#else

	#endif

	int fd;
	switch(Ip_family)
	{
		case CAT_IPV4:
			{
				struct sockaddr_in server_addr;
				fd = Cat_Socket(PF_INET, SOCK_STREAM, 0);
				#ifdef _WINDOWSSYS_

				#else
					bzero(&server_addr, sizeof(server_addr));
				#endif
				if(IP == NULL)
				{
					server_addr.sin_addr.s_addr = INADDR_ANY;
				}
				else
				{
					#ifdef _WINDOWSSYS_
					server_addr.sin_addr.S_un.S_addr = inet_addr(IP);
					/* 
					if(InetPton(AF_INET, (PCWSTR)IP, (PVOID)&server_addr.sin_addr) <= 0)
					{
						Cat_Errexit("InetPton IPv4 error");
					}*/
					#else
					if(inet_pton(AF_INET, IP, &server_addr.sin_addr.s_addr) <= 0)
					{
						Cat_Errexit("inet_pton IPv4 error");
					}
					#endif
				}
				server_addr.sin_family = AF_INET;
				server_addr.sin_port = htons(Port);
				int opt = 1;
				#ifdef _WINDOWSSYS_
					setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
				#else
					setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
				#endif
				int addlen = sizeof(server_addr);
				Cat_Bind(fd, (struct sockaddr *)&server_addr, (void *)&addlen);
				break;
			}
		case CAT_IPV6:
			{
				struct sockaddr_in6 server_addr;
				fd = Cat_Socket(PF_INET6, SOCK_STREAM, 0);
				#ifdef _WINDOWSSYS_

				#else
					bzero(&server_addr, sizeof(server_addr));
				#endif
				if(IP == NULL)
				{
					server_addr.sin6_addr = in6addr_any;
				}
				else
				{
					#ifdef _WINDOWSSYS_
					//server_addr.sin6_addr.__s6_addr32 = inet_addr(IP);
					/* 
					if(InetPton(AF_INET6, (PCWSTR)IP, (PVOID)&server_addr.sin6_addr) <= 0)
					{
						Cat_Errexit("InetPton IPv6 error");
					}*/
					#else
					if(inet_pton(AF_INET6, IP, &server_addr.sin6_addr) <= 0)
					{
						Cat_Errexit("inet_pton IPv6 error");
					}
					#endif
				}
				server_addr.sin6_family = AF_INET6;
				server_addr.sin6_port = htons(Port);
				int opt = 1;
				#ifdef _WINDOWSSYS_
					setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
				#else
					setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
				#endif
				int addlen = sizeof(server_addr);
				Cat_Bind(fd, (struct sockaddr *)&server_addr, (void *)&addlen);
				break;
			}
		default:
			{
				Cat_Errexit("Cat_TcpScoket error");
				return -1;
			}
	}

	return fd;
}


int Cat_UdpScoket(const char* IP, short Port, int Ip_family)
{
	#ifdef _WINDOWSSYS_
		//初始化WSA
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0)
		{
			return 0;
		}
	#endif
	int fd;
	switch(Ip_family)
	{
		case CAT_IPV4:
			{
				struct sockaddr_in server_addr;
				fd = Cat_Socket(PF_INET, SOCK_DGRAM, 0);
				#ifdef _WINDOWSSYS_

				#else
					bzero(&server_addr, sizeof(server_addr));
				#endif
				if(IP == NULL)
				{
					server_addr.sin_addr.s_addr = INADDR_ANY;
				}
				else
				{
					#ifdef _WINDOWSSYS_
					server_addr.sin_addr.S_un.S_addr = inet_addr(IP);
					/* 
					if(InetPton(AF_INET, (PCWSTR)IP, (PVOID)&server_addr.sin_addr) <= 0)
					{
						Cat_Errexit("InetPton IPv4 error");
					}*/
					#else
					if(inet_pton(AF_INET, IP, &server_addr.sin_addr.s_addr) <= 0)
					{
						Cat_Errexit("inet_pton IPv4 error");
					}
					#endif
				}
				server_addr.sin_family = AF_INET;
				server_addr.sin_port = htons(Port);
				int opt = 1;
				#ifdef _WINDOWSSYS_
					setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
				#else
					setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
				#endif
				int addlen = sizeof(server_addr);
				Cat_Bind(fd, (struct sockaddr *)&server_addr, (void *)&addlen);
				
				break;
			}
		case CAT_IPV6:
			{
				struct sockaddr_in6 server_addr;
				fd = Cat_Socket(PF_INET6, SOCK_DGRAM, 0);
				#ifdef _WINDOWSSYS_

				#else
					bzero(&server_addr, sizeof(server_addr));
				#endif
				if(IP == NULL)
				{
					server_addr.sin6_addr = in6addr_any;
				}
				else
				{
					#ifdef _WINDOWSSYS_
					//server_addr.sin6_addr.__s6_addr32 = inet_addr(IP);
					/* 
					if(InetPton(AF_INET6, (PCWSTR)IP, (PVOID)&server_addr.sin6_addr) <= 0)
					{
						Cat_Errexit("InetPton IPv6 error");
					}*/
					#else
					if(inet_pton(AF_INET6, IP, &server_addr.sin6_addr) <= 0)
					{
						Cat_Errexit("inet_pton IPv6 error");
					}
					#endif
				}
				server_addr.sin6_family = AF_INET6;
				server_addr.sin6_port = htons(Port);
				int opt = 1;
				#ifdef _WINDOWSSYS_
					setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
				#else
					setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
				#endif
				int addlen = sizeof(server_addr);
				Cat_Bind(fd, (struct sockaddr *)&server_addr, (void *)&addlen);
				break;
			}
		default:
			{
				Cat_Errexit("Cat_TcpScoket error");
				return -1;
			}
	}

	return fd;
}













