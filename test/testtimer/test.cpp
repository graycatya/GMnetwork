/*************************************************************************
    > File Name: 11-3.cpp
    > Author: Torrance_ZHANG
    > Mail: 597156711@qq.com
    > Created Time: Sun 11 Feb 2018 04:23:50 AM PST
 ************************************************************************/

#include"../../SrcLib/Catdef.h"

#include "CatTimeround.h"
using namespace std;

#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define TIMESLOT 10
#define BUFFER_SIZE 64

struct my_timers
{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    timerevent_t* timer;
};


static int pipefd[2];
static timerwheel_t* timer_lst = NULL;
static int epollfd = 0;

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL, 0);
    int new_option = old_option | O_NONBLOCK;
    printf("new_option %d\n", new_option);
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void sig_handler(int sig) {
    int save_errno = errno;
    int msg = sig;
    send(pipefd[1], (char*)&msg, 1, 0);
    errno = save_errno;
}

void addsig(int sig) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

void timer_handler() {
    Cat_timerwheel_tick(timer_lst);
    alarm(TIMESLOT);
}

void cb_func(void* user_datas) {
    my_timers* user_data = (my_timers*)user_datas;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data -> sockfd, 0);
    assert(user_data);
    close(user_data -> sockfd);
    printf("close fd %d\n", user_data -> sockfd);
}

int main(int argc, char** argv) {
    if(argc <= 2) {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    if(ret == -1) {
        printf("errno is %d\n", errno);
        return 1;
    }
    ret = listen(listenfd, 5);
    assert(ret != -1);
    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd, listenfd);

    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
    assert(ret != -1);
    setnonblocking(pipefd[1]);
    addfd(epollfd, pipefd[0]);
    
    addsig(SIGALRM);
    addsig(SIGTERM);
    addsig(SIGQUIT);
    bool stop_server = false;
    timer_lst = Cat_timerwheel_create(30, TIMESLOT);
    struct my_timers* users = new struct my_timers[FD_LIMIT];
    bool timeout = false;
    alarm(TIMESLOT);

    while(!stop_server) {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if((number < 0) && (errno != EINTR)) {
            printf("epoll failure\n");
            break;
        }
        for(int i = 0; i < number; i ++) {
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd) {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(address);
                int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlength);
                addfd(epollfd, connfd);
                users[connfd].address = client_address;
                users[connfd].sockfd = connfd;
                printf("add_timer... \n");
                timerevent_t* timer = Cat_add_timerevent(timer_lst, TIMESLOT);
                timer -> arg = &users[connfd];
                timer -> function = cb_func;
                users[connfd].timer = timer;
                /* 
                time_t cur = time(NULL);
                timer -> expire = cur + 3 * TIMESLOT;
                */
                
            }
            else if((sockfd == pipefd[0]) && (events[i].events & EPOLLIN)) {
                int sig;
                char signals[1024];
                ret = recv(pipefd[0], signals, sizeof(signals), 0);
                if(ret == -1) continue;
                else if(ret == 0) continue;
                else {
                    for(int i = 0; i < ret; i ++) {
                        switch(signals[i]) {
                            case SIGALRM: {
                                timeout = true;
                                break;
                            }
                            case SIGTERM: {
                                stop_server = true;
                            }
                        }
                    }
                }
            }
            else if(events[i].events & EPOLLIN) {
                memset(users[sockfd].buf, 0, BUFFER_SIZE);
                ret = recv(sockfd, users[sockfd].buf, BUFFER_SIZE, 0);
                printf("get %d bytes of client data %s from %d\n", ret, users[sockfd].buf, sockfd);

                timerevent_t* timer = users[sockfd].timer;
                if(ret < 0) {
                    if(errno != EAGAIN) {
                        cb_func(&users[sockfd]);
                        if(timer) {
                            Cat_del_timerevent(timer_lst, timer);
                            //timer_lst.del_timer(timer);
                        }
                    }
                }
                else if(ret == 0) {
                    cb_func(&users[sockfd]);
                    if(timer) {
                        Cat_del_timerevent(timer_lst, timer);
                        //timer_lst.del_timer(timer);
                    }
                }
                else {
                    if(timer) {
                        Cat_del_timerevent(timer_lst, timer);
                        timer = Cat_add_timerevent(timer_lst, TIMESLOT);
                        timer->arg = &users[sockfd];
                        timer->function = cb_func;
                        users[sockfd].timer = timer;
                    }
                }
            }
            else {}
        }
        if(timeout) {
            timer_handler();
            timeout = false;
        }
    }
    printf("stop %d\n", stop_server);
    close(listenfd);
    close(pipefd[1]);
    close(pipefd[0]);
    delete [] users;
    Cat_timerwheel_delete(timer_lst);
    return 0;
}