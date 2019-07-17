#include "Cat_Epoll.h"



CATEPOLL::CatEpoll::CatEpoll(int EpollSize , int WaitSize, int EventSize)
{
    epfd = epoll_create(EpollSize);
    if(epfd < 0)
    {
        Cat_Errexit("CatEpoll epfd errno ");
    }
    this->WaitSize = WaitSize;
    this->events = new epoll_event[EventSize];

}

CATEPOLL::CatEpoll::~CatEpoll()
{
    if(this->events != nullptr)
    {
        delete[] this->events;
        this->events = nullptr;
    }
}

int CATEPOLL::CatEpoll::Wait(int timeout)
{
    int ret = 0;
    if((ret = epoll_wait(epfd, events, WaitSize, timeout)) < 0)
    {
        Cat_Errexit("CatEpoll Wait errno ");
    }
    return ret;
}

int CATEPOLL::CatEpoll::Ctl(int op, int fd, epoll_event *event)
{
    return epoll_ctl(epfd, op, fd, event);
}

int CATEPOLL::CatEpoll::Addfd(int fd, int event, bool fl)
{
    epoll_event event_t;
    event_t.events = event;
    event_t.data.fd = fd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event_t);
    if(fl)
    { 
        set_fl(fd, O_NONBLOCK);
    }
    return ret;
}

int CATEPOLL::CatEpoll::Modifyfd(int fd, int event, bool fl)
{
    epoll_event event_t;
    event_t.events = event;
    event_t.data.fd = fd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event_t);
    if(fl)
    { 
        set_fl(fd, O_NONBLOCK);
    }
    return ret;
}

int CATEPOLL::CatEpoll::Deletefd(int fd, int event)
{
    epoll_event event_t;
    event_t.events = event;
    event_t.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event_t);
}

void CATEPOLL::CatEpoll::InitNetwork(const char* ip, short port, int listens, int sock_family)
{
    this->ip = ip;
    this->port = port;
    this->listens = listens;
    this->sock_family = sock_family;
}

int CATEPOLL::CatEpoll::NetworkStart(NETWORK_MOD mod)
{
    switch(mod)
    {
        case EPOLLTCP:
        {
            lfd = TcpScoket(ip, port, sock_family);
            Listen(lfd, listens);
            break;
        }
        case EPOLLUDP:
        {
            lfd = UdpScoket(ip, port, sock_family);
            Listen(lfd, listens);
            break;
        }
        default:
            return -1;
    }
    return Addfd(lfd, EPOLLIN | EPOLLET | EPOLLRDHUP);
}

int CATEPOLL::CatEpoll::NetworkClose(int sockfd, int num)
{
    close(sockfd);
    return Ctl(EPOLL_CTL_DEL, sockfd, &events[num]);
}

epoll_event CATEPOLL::CatEpoll::GetEvent(int num)
{
    return events[num];
}

int CATEPOLL::CatEpoll::GetLfd(void)
{
    return lfd;
}