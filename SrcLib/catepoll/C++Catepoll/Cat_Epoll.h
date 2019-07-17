#pragma once

#include "../../Network/C++Catnet/Cat_net.h"

namespace CATEPOLL
{

class CatEpoll : public CATNET::CatNet
{
public:
    enum NETWORK_MOD { EPOLLTCP, EPOLLUDP };
    CatEpoll(int EpollSize = 1, int WaitSize = 1024, int EventSize = 1024);
    ~CatEpoll();
    int Wait(int timeout = -1);

    int Ctl(int op, int fd, epoll_event *event);
    int Addfd(int fd, int event, bool fl = false);
    int Modifyfd(int fd, int event, bool fl = false);
    int Deletefd(int fd, int event);

    void InitNetwork(const char* ip = nullptr, short port = 9600, int listens = 5, int sock_family = AF_INET);
    int NetworkStart(NETWORK_MOD mod = EPOLLTCP);
    int NetworkClose(int sockfd, int num);
    epoll_event GetEvent(int num);
    int GetLfd(void);

private:
    int WaitSize;
    epoll_event* events;
    int epfd;
    int lfd;

    const char* ip;
    short port;
    int sock_family;
    int listens;

    void* (*function)(void *arg);
    void* arg;

};



}
