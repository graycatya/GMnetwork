#ifndef __CATDEF_H_
#define __CATDEF_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>

#ifdef _WINDOWSSYS_ 
#include <ws2tcpip.h>
#include <winsock.h>
#include <WinSock2.h>

#else
#include <time.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/select.h>
#endif

#define _OUT(format, ...) printf((format), ##__VA_ARGS__)

#define ERROR(format, ...) \
do { \
	_OUT("[ERROR]%s %s(Line %d): ", __FILE__, __FUNCTION__, __LINE__); \
	_OUT((format), ##__VA_ARGS__); \
} while(0)

#endif

