#include "Catepoll.h"
#include "catthreadpool.h"

struct epollpool_s
{
    int num;
    Catepoll_t* cev;
};

void *process(void* arg)
{
    struct epollpool_s *cevs = (struct epollpool_s*)arg;
    while(1)
	{
        char buf[6]="";
        int n = Cat_Read(cevs->cev->events[cevs->num].data.fd , buf,5);
        if(n < 0 )
        {
            if(errno ==  EAGAIN)//缓冲区如果读干净了,read返回值小于0,errno设置成EAGAIN
                break;
            Catepollx_close(cevs->cev->epfd, cevs->cev->events[cevs->num].data.fd, &cevs->cev->events[cevs->num]);
            printf("client close\n");
            break;
        }
        else if(n == 0)
        {
            Catepollx_close(cevs->cev->epfd, cevs->cev->events[cevs->num].data.fd, &cevs->cev->events[cevs->num]);
            printf("client close\n");
            break;
        }
        else
        {
            printf("%s\n",buf );
            Cat_Send(cevs->cev->events[cevs->num].data.fd ,buf,n);
        }
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        return -1;
    }
    threadpool_t *thp = threadpool_create(10, 500, 200);
    Catepoll_t* catepoll_t = (Catepoll_t*)malloc(sizeof(Catepoll_t));
    struct epollpool_s my_epollpool;
    catepoll_t->ip = argv[1];
    catepoll_t->port = atoi(argv[2]);
    catepoll_t->EpollSize = 1;
    catepoll_t->listens = 128;
    catepoll_t->maxevents = 1024;
    catepoll_t->WaitSize = 1024;
    catepoll_t->events = NULL;
    catepoll_t->sock_family = CAT_IPV4;
    printf(" %s start ..\n", catepoll_t->ip);
    Catepollx_TcpStart(catepoll_t);
    while(1)
    {
        int nfds = Catepoll_Wait(catepoll_t->epfd, catepoll_t->events, catepoll_t->maxevents, -1);
        if(nfds == 0)
        {
            continue;
        }
        for(int i = 0; i < nfds; i++)
        {
            if(catepoll_t->events[i].data.fd == catepoll_t->lfd && catepoll_t->events[i].events & EPOLLIN)
            {
				struct sockaddr_in cliaddr;
				char buf_ip[16]="";
				socklen_t len  = sizeof(cliaddr);
				int cfd = Cat_Accept(catepoll_t->lfd,(struct sockaddr *)&cliaddr,&len, 0);
				printf("client ip=%s port=%d\n",inet_ntop(AF_INET,
				    &cliaddr.sin_addr.s_addr,buf_ip,sizeof(buf_ip)),
                    ntohs(cliaddr.sin_port));
                Catsetnonblocking(cfd);
                Catepoll_addfd(catepoll_t->epfd, cfd, EPOLLIN | EPOLLET);
            }
            else if(catepoll_t->events[i].events & EPOLLIN)//普通的读事件
            {
                    my_epollpool.num = i;
                    my_epollpool.cev = catepoll_t;
                    threadpool_add(thp, process, (void *)&my_epollpool);
            }
        }
    }
    Catepollx_delete(catepoll_t);
    threadpool_destroy(thp);
    return 0;
}