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
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#endif

