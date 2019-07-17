#include "Catepoll.h"


int Catepoll_Init(int EpollSize, int WaitSize)
{
    int epfd = epoll_create(EpollSize);
    if(epfd < 0)
    {
        return -2;
    }
    return epfd;
}

int Catepoll_Wait(int epfd, struct epoll_event* events, int maxevents, int timeout)
{
    int ret = 0;
    if((ret = epoll_wait(epfd, events, maxevents, timeout)) < 0)
    {
        Cat_Errexit("Catepoll_Wait error");
    }
    return ret;
}

int Catepoll_Ctl(int epfd, int op, int fd, struct epoll_event *events)
{
    return epoll_ctl(epfd, op, fd, events);
}

int Catepoll_addfd(int epfd, int fd, int events)
{
	struct epoll_event event;
	event.events = events;
	event.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    set_fl(fd, O_NONBLOCK);
    return 0;
}

int Catepoll_modifyfd(int epfd, int fd, int events)
{
	struct epoll_event event;
	event.events = events;
	event.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}

int Catepoll_deletefd(int epfd, int fd, int events)
{
	struct epoll_event event;
	event.events = events;
	event.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);
}

int Catepollx_TcpStart(Catepoll_t* catepoll_t)
{
    int lfd = Cat_TcpScoket(catepoll_t->ip, catepoll_t->port, catepoll_t->sock_family);
    Cat_Listen(lfd, catepoll_t->listens);
    catepoll_t->lfd = lfd;
    catepoll_t->events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * catepoll_t->WaitSize);
    if((catepoll_t->epfd = Catepoll_Init(catepoll_t->EpollSize, catepoll_t->WaitSize)) < 0)
    {
        Cat_Errexit("Catepoll_Init error");
    }
    return Catepoll_addfd(catepoll_t->epfd, lfd, EPOLLIN | EPOLLET | EPOLLRDHUP);
}

int Catepollx_UdpStart(Catepoll_t* catepoll_t)
{
    int lfd = Cat_UdpScoket(catepoll_t->ip, catepoll_t->port, catepoll_t->sock_family);
    catepoll_t->lfd = lfd;
    catepoll_t->events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * catepoll_t->WaitSize);
    if((catepoll_t->epfd = Catepoll_Init(catepoll_t->EpollSize, catepoll_t->WaitSize)) < 0)
    {
        Cat_Errexit("Catepoll_Init error");
    }
    return Catepoll_addfd(catepoll_t->epfd, lfd, EPOLLIN);
}

int Catepollx_delete(Catepoll_t* catepoll_t)
{
    if(catepoll_t != NULL)
    {
        if(catepoll_t->events != NULL)
        {
            free(catepoll_t->events);
            catepoll_t->events = NULL;
        }

        free(catepoll_t);
        catepoll_t = NULL;
    }
    else
    {
        return -1;
    }
    return 0;
    
}


int Catepollx_close(int epfd, int sockfd, struct epoll_event *events)
{
    int ret = -1;
    close(sockfd);
    ret = epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, events);
    return ret;
}

/* 
int Catepollx_TcpStart(Catepoll_t* catepoll_t)
{
    switch(catepoll_t->sock_family)
    {
        case CAT_IPV4:
        {
            struct sockaddr_in address;
            bzero(&address, sizeof(address));
            socklen_t len = sizeof(struct sockaddr_in);
            address.sin_family = AF_INET;
            if(IP == NULL)
			{
				server_addr.sin_addr.s_addr = INADDR_ANY;
			}
            else
            {
                inet_pton(AF_INET, catepoll_t->ip, &address.sin_addr.s_addr);
            }
            address.sin_port = htons(port);

        }
        case CAT_IPV6:
        {
            address.sin_family = AF_INET6;
            inet_pton(AF_INET6, catepoll_t->ip, &address.sin_addr);
        }
        default:
        {
            struct sockaddr_in6 address;
            bzero(&address, sizeof(struct sockaddr_in6));
            if(IP == NULL)
			{
				server_addr.sin6_addr = in6addr_any;
			}
            return -1;
        }
    }
    

    

}

int Catepollx_UdpStart(Catepoll_t* catepoll_t)
{

}

int Catepollx_delete(Catepoll_t* catepoll_t)
{

}*/