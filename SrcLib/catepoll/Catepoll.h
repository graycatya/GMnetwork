#ifndef CATEPOLL_H
#define CATEPOLL_H

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

int Catepoll_Init(int EpollSize, int WaitSize);

int Catepoll_Wait(int epfd, struct epoll_event* events, int maxevents, int timeout);

int Catepoll_Ctl(int epfd, int op, int fd, struct epoll_event *event);

int Catepoll_close(int epfd, int sockfd);



#endif