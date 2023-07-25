#include "socket_accept_operation.h"

#include "const.h"
#include "sys/socket.h"
#include "socket.h"
#include "epoll_agent.h"
#include <iostream>

SocketAccpetOperation::SocketAccpetOperation(Socket* s):socket(s) {
    // s->agent->WatchWrite(s);
}

SocketAccpetOperation::~SocketAccpetOperation() {
//    socket->agent->WatchWrite(socket);
}


int SocketAccpetOperation::syscall() {
    // std::cout << "accept" << std::endl;
    return accept(socket->sockfd,(struct sockaddr*)(&socket->sin),&(socket->socklen));
}

void SocketAccpetOperation::suspend() {
    socket->_recvcor = _awaitingCoroutine;
}