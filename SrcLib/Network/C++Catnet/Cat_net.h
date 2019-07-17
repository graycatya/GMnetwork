#pragma once

#include "../../Catdef.h"


namespace CATNET
{
    class CatNet
    {
        public:
            enum { CAT_IPV4, CAT_IPV6 };
            CatNet(){}
            ~CatNet(){}

            int Socket(int family = AF_INET, int sockettype = SOCK_STREAM, int protocol = 0);

            int Bind(int fd, struct sockaddr *addr, void* addrlen);

            int Listen(int fd, int backlog = 5);

            int Accept(int fd, struct sockaddr *addr, void* addrlen, int connects);

            int Connect(int fd, const struct sockaddr *addr, void* addrlen);

            int Close(int fd);

            ssize_t Read(int fd, void *buf, size_t len);

            ssize_t Send(int fd, const void *buf, size_t len);

            ssize_t Reads(int fd, void *buf, size_t len);

            ssize_t Sends(int fd, const void *buf, size_t len);

            ssize_t Read(int fd, void *buf, size_t len, struct sockaddr* src_addr, void* addrlen);

            ssize_t Send(int fd, const void *buf, size_t len, const struct sockaddr* dest_addr, void* addrlen);

            ssize_t Reads(int fd, void *buf, size_t len, struct sockaddr* src_addr, void* addrlen);

            ssize_t Sends(int fd, const void *buf, size_t len, const struct sockaddr* dest_addr, void* addrlen);

            int TcpScoket(const char* IP = nullptr, short Port = 9600, int Ip_family = CAT_IPV4);

            int UdpScoket(const char* IP = nullptr, short Port = 9800, int Ip_family = CAT_IPV4);

        private:
            int fd;

    };
}