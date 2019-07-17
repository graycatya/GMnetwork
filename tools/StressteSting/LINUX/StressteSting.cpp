#include "../../../SrcLib/catepoll/C++Catepoll/Cat_Epoll.h"
#include "../../../SrcLib/Threadpool/C++Catthreadpool/Threadpool.h"
#include "Catnet.h"
#include <pthread.h>

using namespace CATEPOLL;
using namespace CATTHREADPOOL;
using namespace CATNET;

pthread_mutex_t my_lock;

static const char* request = "StressteSting test tools ....";

struct my_event
{
    CatEpoll* my_epoll = nullptr;
    CatThreadPool* my_threadpool = nullptr;
    int num = 0;
    sockaddr_in address;
    socklen_t len;
};


void* Start_connect(void* arg)
{
    my_event* event = (my_event*)arg;
    pthread_mutex_lock(&my_lock);
    int sockfd = Cat_Socket(PF_INET, SOCK_STREAM, 0);
    Cat_Connect(sockfd, (struct sockaddr *)&event->address, (void *)&event->len);
	event->my_epoll->Addfd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR, true );
    pthread_mutex_unlock(&my_lock);
    return nullptr;
}

void* process(void* arg)
{
    pthread_mutex_lock(&my_lock);
    my_event* event = (my_event*)arg;
    epoll_event evne = event->my_epoll->GetEvent(event->num);
    uint32_t events = evne.events;
    pthread_mutex_unlock(&my_lock);
    int sockfd = evne.data.fd;
    char buffer[2048];
    if(events & EPOLLIN)
    {
        if(!Cat_Read(sockfd, buffer, 2048))
        {
            event->my_epoll->Deletefd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR);
        }
        event->my_epoll->Modifyfd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR);
    }
    else if(events & EPOLLOUT)
    {
        if(!Cat_Send(sockfd, request, strlen(request)) )
        {
            event->my_epoll->Deletefd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR);
        }
        event->my_epoll->Modifyfd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR);

    }
    else if(events & EPOLLERR)
    {
        event->my_epoll->Deletefd(sockfd, EPOLLOUT | EPOLLET | EPOLLERR);
    }
    return nullptr;
}

int main(int argc, char* argv[])
{
    if(argc < 4)
    {return -1;}
    pthread_mutex_init(&my_lock, NULL);
    my_event* event = new my_event;
    event->my_epoll = new CatEpoll(1, 10000, 10000);
    event->my_threadpool = new CatThreadPool(1000, 50, 30);
	bzero(&event->address, sizeof(event->address));
	event->len = sizeof(struct sockaddr_in);
	event->address.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &event->address.sin_addr);
	event->address.sin_port = htons(atoi(argv[2]));
    event->my_threadpool->SetTickTime(2);
    /* for(event->num = 0; event->num < atoi(argv[3]); event->num++ )
    {
        event->my_threadpool->AddThreads(Start_connect, (void*)event);
    }*/


    for(event->num = 0; event->num < atoi(argv[3]); event->num++ )
    {
        //sleep(1);
        usleep(5000);
        Start_connect((void*)event);
    }

    while(event->my_threadpool->GetBusyThreadNum() > 0)
    {
        usleep(500);
    }


    int nfds = event->my_epoll->Wait(0);
    if(nfds <= 0)
    {
       return -1;
    }
    for(event->num = 0; event->num < nfds; event->num++)
    {
        //event->my_threadpool->AddThreads(process, (void*)event);
        process(event);
    }
/* 
    while(event->my_threadpool->GetBusyThreadNum() > 0)
    {
        usleep(500);
    }
*/
    std::cout << "StressteSting exit." << std::endl;
    delete event->my_epoll;
    delete event->my_threadpool;
    delete event;
    pthread_mutex_destroy(&my_lock);
    return 0;
}