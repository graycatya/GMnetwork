#ifndef __CATDEF_H_
#define __CATDEF_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>

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
#include <signal.h>

/* 设置非阻塞 */
int Catsetnonblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

void set_fl(int fd, int flags)
{
	int		val;

	// 获取标志
	if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
		//error_ret("fcntl get error");
	}

	val |= flags;

	// 设置标志
	if (fcntl(fd, F_SETFL, val) < 0) {
		//error_ret("fcntl set error");
	}
}

//typedef struct epoll_event epoll_event;
#endif

#define _OUT(format, ...) printf((format), ##__VA_ARGS__)

#define ERROR(format, ...) \
do { \
	_OUT("[ERROR]%s %s(Line %d): ", __FILE__, __FUNCTION__, __LINE__); \
	_OUT((format), ##__VA_ARGS__); \
} while(0)

#endif

