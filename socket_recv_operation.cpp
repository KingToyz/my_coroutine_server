#include "socket_recv_operation.h"

#include "const.h"
#include "sys/socket.h"
#include "socket.h"
#include "block_syscall.h"
#include "epoll_agent.h"
#include <iostream>

SocketRecvOperation::SocketRecvOperation(void* buffer,Socket* s,int len):len(len),buffer(buffer),socket(s) {
    // s->agent->WatchRead(s);
}

SocketRecvOperation::~SocketRecvOperation() {
    // socket->agent->UnWatchRead(socket);
}


int SocketRecvOperation::syscall() {
    std::cout << "recv" << std::endl;
    return recv(socket->sockfd,(buffer),len,0);
}

void SocketRecvOperation::suspend() {
    socket->_recvcor = _awaitingCoroutine;
}