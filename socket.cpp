#include "socket.h"
#include <string.h>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include "epoll_agent.h"
#include <coroutine>
#include <iostream>
#include <sys/timerfd.h>
#include <sys/time.h>

void SetNonBlock(int fd) {
    int fl = fcntl(fd, F_GETFL); //文件描述符的属性取出来存入fl中
    if (fl < 0)
    { //执行失败返回-1并报错
        perror("fcntl");
        return;
    }
    fcntl(fd, F_SETFL, fl | O_NONBLOCK); //设置fl | O_NONBLOCK 类似位图填充类型设置
}

Socket::~Socket() {
    agent->Detach(this);
    std::cout << "close " << sockfd << std::endl;
    close(sockfd);
}

Socket::Socket(std::string port,EpollAgent* agent):agent(agent),IOState(0),parser(agent),isclose(false) {
    socklen = sizeof(struct sockaddr);
    memset(&sin, 0, socklen);
    sin.sin_family      = AF_INET;
    sin.sin_port    = htons(std::atoi(port.c_str()));
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > sockfd)
    {
        perror("socket");
        exit(1);
    }
    SetNonBlock(sockfd);
    // std::cout << "fd:" << fd << std::endl;
    int one = 1;
    if (0 > setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) 
    {
        perror("setsockopt"); 
        close(sockfd);
        exit(1); 
    }
    if (0 > bind(sockfd, (struct sockaddr *)&sin, socklen)) 
    { 
        perror("bind"); 
        close(sockfd);
        exit(1); 
    }

    if (0 > listen(sockfd, 5)) 
    { 
        perror("listen"); 
        close(sockfd);
        exit(1); 
    }
    std::cout << "fd:" << sockfd << " construct!" << std::endl;
    agent->Attch(this);
    agent->WatchRead(this);
}


Socket::Socket(int fd,EpollAgent* agent):agent(agent),sockfd(fd),IOState(0),parser(agent),isclose(false) { 
    SetNonBlock(fd);
    agent->Attch(this);
    agent->WatchRead(this);
    std::cout << "fd:" << fd << " construct!" << std::endl;
}


task<Socket*> Socket::accept() {
    int fd = co_await SocketAccpetOperation(this);
    if(fd == -1) {
        std::cout << "accpet error" << std::endl;
        exit(-1);
    }
    co_return new Socket(fd,agent);
}

SocketRecvOperation Socket::recv(void* buffer, std::size_t len) {
    return SocketRecvOperation(buffer,this,len);
}


SocketWriteOperation Socket::write(void* buffer ,std::size_t len) {
    return SocketWriteOperation((char*)buffer,this,len);
}


SocketTimerOperation Socket::read() {
    return SocketTimerOperation(this);
}


task<bool> Socket::loop() {

    ssize_t bRecv = 0;
    ssize_t bSend = 0;
    while (bRecv >= 0) {
        char buffer[1024] = {0};
        bRecv = co_await recv(buffer,sizeof buffer);
        if (bRecv > 0) {
            auto ret = co_await parser.Execute(buffer,bRecv);
            if(ret.first != nullptr) {
                while (bSend < ret.first->GetLen()- ret.first->GetWritePos()) {
                    ssize_t res = co_await write((void*)(ret.first->GetData()+ret.first->GetWritePos()),ret.first->GetLen()-ret.first->GetWritePos());
                    if(res < 0) {
                        std::cout << "false1\n";
                        co_return false;
                    }
                    ret.first->SetHasWrite(res);
                }
                isclose = true;
                std::cout << "false2\n";
                co_return false;
            }
        }
    }
    std::cout << "false3\n";
    co_return false;
}