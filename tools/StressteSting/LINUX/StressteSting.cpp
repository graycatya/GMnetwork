#include "../../../SrcLib/catepoll/C++Catepoll/Cat_Epoll.h"
#include "../../../SrcLib/Threadpool/C++Catthreadpool/Threadpool.h"
#include "Catnet.h"
using namespace CATEPOLL;
using namespace CATTHREADPOOL;
using namespace CATNET;

static const char* request = "StressteSting test tools ....";

struct my_event
{
    CatEpoll* my_epoll = nullptr;
    CatThreadPool* my_threadpool = nullptr;
    int num = 0;
    const char* ip = nullptr;
    int port;
};


void* Start_connect(void* arg)
{
    my_event* event = (my_event*)arg;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	socklen_t len = sizeof(struct sockaddr_in);
	address.sin_family = AF_INET;
	inet_pton(AF_INET, event->ip, &address.sin_addr);
	address.sin_port = htons(event->port);

    int sockfd = Cat_Socket(PF_INET, SOCK_STREAM, 0);
    Cat_Connect(sockfd, (struct sockaddr *)&address, (void *)&len);
	event->my_epoll->Addfd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR, true );
    return nullptr;
}

void* process(void* arg)
{
    my_event* event = (my_event*)arg;
    epoll_event evne = event->my_epoll->GetEvent(event->num);
    int sockfd = evne.data.fd;
    char buffer[2048];
    if(evne.events & EPOLLIN)
    {
        if(!Cat_Read(sockfd, buffer, 2048))
        {
            event->my_epoll->Deletefd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR);
        }
        event->my_epoll->Modifyfd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR);
    }
    else if(evne.events & EPOLLOUT)
    {
        if(!Cat_Send(sockfd, request, strlen(request)) )
        {
            event->my_epoll->Deletefd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR);
        }
        event->my_epoll->Modifyfd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR);

    }
    else if(evne.events & EPOLLERR)
    {
        event->my_epoll->Deletefd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR);
    }
    return nullptr;
}

int main(int argc, char* argv[])
{
    if(argc < 4)
    {return -1;}
    my_event* event = new my_event;
    event->my_epoll = new CatEpoll(1, 10000, 10000);
    event->my_threadpool = new CatThreadPool(1000, 50, 30);

    event->ip = (const char*)argv[1];
    event->port = atoi(argv[2]);
    for(event->num = 0; event->num < atoi(argv[3]); event->num++ )
    {
        event->my_threadpool->AddThreads(Start_connect, (void*)event);
    }
    while(1)
    {
         int nfds = event->my_epoll->Wait(-1);
        if(nfds == 0)
        {
            continue;
        }
        for(event->num = 0; event->num < nfds; event->num++)
        {
            event->my_threadpool->AddThreads(process, (void*)event);
        }
    }
    delete event->my_epoll;
    delete event->my_threadpool;
    delete event;
    return 0;
}