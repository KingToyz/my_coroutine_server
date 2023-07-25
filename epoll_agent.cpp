#include "epoll_agent.h"
#include "socket.h"
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


EpollAgent::EpollAgent() {
    Epollfd = epoll_create1(0);
    if (0 > Epollfd) 
    { 
        perror("epoll_create1"); 
        exit(1); 
    }
    // epev.events = EPOLLIN | EPOLLET;
    // epev.data.fd = exitfd[0]; // for quit
    // int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, exitfd[0], &epev);
    // if (0 > ret) 
    // { 
    //     perror("epoll_ctl_exitfd");exit(1); 
    // }
    
}

void EpollAgent::UnWatchRead(Socket* s) {
    s->NewIOState = s->IOState & (~EPOLLIN);
    processSocket.insert(s);
}

void EpollAgent::WatchRead(Socket* s) {
    s->NewIOState = s->IOState | EPOLLIN;
    processSocket.insert(s);
}


void EpollAgent::UnWatchWrite(Socket* s) {
    s->NewIOState = s->IOState & (~EPOLLOUT);
    processSocket.insert(s);
}


void EpollAgent::WatchWrite(Socket* s) {
    s->NewIOState = s->IOState | EPOLLOUT;
    processSocket.insert(s);
}

void EpollAgent::Attch(Socket* s) {
    struct epoll_event ev;
    auto io_state = EPOLLIN | EPOLLET;
    ev.events = io_state;
    ev.data.ptr = s;
    if (epoll_ctl(Epollfd, EPOLL_CTL_ADD, s->sockfd, &ev) == -1)
        perror("epoll_ctl_add");
}


void EpollAgent::Detach(Socket* s) {

    if (epoll_ctl(Epollfd, EPOLL_CTL_DEL,s->sockfd, nullptr) == -1) {
        perror("epoll_ctl_del");
    }
    processSocket.erase(s);
}


void EpollAgent::Run() {
        struct epoll_event ev, events[10240];
    for (;;)
    {
        auto nfds = epoll_wait(Epollfd, events, 1024, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int n = 0; n < nfds; ++n)
        {
            auto socket = static_cast<Socket*>(events[n].data.ptr); 

            if (events[n].events & EPOLLIN)
                socket->resumeRecv(); // 如果是accpet 
            if (events[n].events & EPOLLOUT)
                socket->resumeSend();
        }
        for (auto* iter : processSocket)
        {
            auto io_state = iter->NewIOState;
            if (iter->IOState == io_state)
                continue;
            ev.events = io_state;
            ev.data.ptr = iter;
            if (epoll_ctl(Epollfd, EPOLL_CTL_MOD, iter->sockfd, &ev) == -1) {
                perror("epoll_ctl_mod");
                continue;
            }
            iter->IOState = io_state;
        }
    }
}