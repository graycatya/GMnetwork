#include "../../../SrcLib/catepoll/C++Catepoll/Cat_Epoll.h"

using namespace CATEPOLL;

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        return -1;
    }
    CatEpoll* myepoll = new CatEpoll(0,1024, 1024);
    myepoll->InitNetwork(argv[1], atoi(argv[2]), 5, CatEpoll::CAT_IPV4);
    myepoll->NetworkStart(CatEpoll::NETWORK_MOD::EPOLLTCP);
    while(1)
    {
        int nfds = myepoll->Wait(-1);
        if(nfds == 0)
        {
            continue;
        }
        for(int i = 0; i < nfds; i++)
        {
            epoll_event event = myepoll->GetEvent(i);
            if(event.data.fd == myepoll->GetLfd() && event.events & EPOLLIN)
            {
				struct sockaddr_in cliaddr;
				char buf_ip[16]="";
				socklen_t len  = sizeof(cliaddr);
				int cfd = myepoll->Accept(myepoll->GetLfd(),(struct sockaddr *)&cliaddr,&len, 0);
				printf("client ip=%s port=%d\n",inet_ntop(AF_INET,
				    &cliaddr.sin_addr.s_addr,buf_ip,sizeof(buf_ip)),
                    ntohs(cliaddr.sin_port));
                //Catsetnonblocking(cfd);
                myepoll->Addfd(cfd, EPOLLIN | EPOLLET, true);
            }
            else if(event.events & EPOLLIN)//普通的读事件
            {
					while(1)
					{
						char buf[6]="";
						int n = myepoll->Read(event.data.fd , buf,5);
						if(n < 0 )
						{
							if(errno ==  EAGAIN)//缓冲区如果读干净了,read返回值小于0,errno设置成EAGAIN
							    break;
                            myepoll->NetworkClose(event.data.fd, i);
							printf("client close\n");
							break;
						}
						else if(n == 0)
						{
							myepoll->NetworkClose(event.data.fd, i);
							printf("client close\n");
							break;

						}
						else
						{
							printf("%s\n",buf );
							myepoll->Send(event.data.fd ,buf,n);
						}
					}
            }
        }
    }
    delete myepoll;
    return 0;
}