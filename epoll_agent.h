#pragma once
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unordered_set>
class Socket;

class EpollAgent {
    public:
        EpollAgent();

        void Run();
        int Epollfd;
        void WatchRead(Socket* s);
        void UnWatchRead(Socket* s);
        void WatchWrite(Socket* s);
        void UnWatchWrite(Socket* s);
        void Attch(Socket* s);
        void Detach(Socket* s);
        struct epoll_event epev;
        struct epoll_event evlist[1024];
        struct sockaddr_in ssin;
        socklen_t socklen;
        std::unordered_set<Socket*>processSocket;
};