#ifndef __CATNET_H_
#define __CATNET_H_

#include "../Catdef.h"

enum {
CAT_IPV4,
CAT_IPV6
};


int Cat_Socket(int family, int sockettype, int protocol);

int Cat_Bind(int fd, struct sockaddr *addr, void* addrlen);

int Cat_Listen(int fd, int backlog);

int Cat_Accept(int fd, struct sockaddr *addr, void* addrlen, int connects);

int Cat_Connect(int fd, const struct sockaddr *addr, void* addrlen);

int Cat_Close(int fd);

ssize_t Cat_Read(int fd, void *buf, size_t len);

ssize_t Cat_Send(int fd, const void *buf, size_t len);

ssize_t Cat_Reads(int fd, void *buf, size_t len);

ssize_t Cat_Sends(int fd, const void *buf, size_t len);

ssize_t Cat_UdpRead(int fd, void *buf, size_t len, struct sockaddr* src_addr, void* addrlen);

ssize_t Cat_UdpSend(int fd, const void *buf, size_t len, const struct sockaddr* dest_addr, void* addrlen);

ssize_t Cat_UdpReads(int fd, void *buf, size_t len, struct sockaddr* src_addr, void* addrlen);

ssize_t Cat_UdpSends(int fd, const void *buf, size_t len, const struct sockaddr* dest_addr, void* addrlen);

int Cat_TcpScoket(const char* IP, short Port, int Ip_family);

int Cat_UdpScoket(const char* IP, short Port, int Ip_family);


#endif
