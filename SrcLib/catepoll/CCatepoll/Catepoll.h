#ifndef CATEPOLL_H
#define CATEPOLL_H

#include "../../Network/CCatnet/Catnet.h"

/*
struct epoll_event
{
    __uint32_t events;  //epoll事件
    epoll_data_t data;  //用户数据
};

typedef union epoll_data
{
    void* ptr;
    int fd;
    uint32_t u32;
    uint64_t u64; 
} epoll_data_t;
*/

struct _Catepoll_t
{

    int EpollSize;
    int WaitSize;
    int maxevents;
    int timeout;

    struct epoll_event* events;
    int epfd;
    int lfd;

    char* ip;
    short port;
    int sock_family;
    int listens;

    void* (*function)(void *arg);
    void* arg;

};

typedef struct _Catepoll_t Catepoll_t;

int Catepoll_Init(int EpollSize, int WaitSize);

int Catepoll_Wait(int epfd, struct epoll_event* events, int maxevents, int timeout);

int Catepoll_Ctl(int epfd, int op, int fd, struct epoll_event *event);

int Catepoll_addfd(int epfd, int fd, int events);

int Catepoll_modifyfd(int epfd, int fd, int events);

int Catepoll_deletefd(int epfd, int fd, int events);

int Catepollx_TcpStart(Catepoll_t* catepoll_t);

int Catepollx_UdpStart(Catepoll_t* catepoll_t);

int Catepollx_delete(Catepoll_t* catepoll_t);

int Catepollx_close(int epfd, int sockfd, struct epoll_event *events);

#endif