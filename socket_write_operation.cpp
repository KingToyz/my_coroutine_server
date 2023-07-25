#include "socket_write_operation.h"
#include "const.h"
#include "sys/socket.h"
#include "socket.h"
#include "block_syscall.h"
#include "epoll_agent.h"
#include <iostream>

SocketWriteOperation::SocketWriteOperation(void* buffer,Socket* s,int len):len(len),buffer(buffer),socket(s) {
    // s->agent->WatchWrite(s);
}

SocketWriteOperation::~SocketWriteOperation() {
    // socket->agent->UnWatchWrite(socket);
}


int SocketWriteOperation::syscall() {
    // std::cout << "send" << std::endl;
    return send(socket->sockfd,(buffer),len,0);
}

void SocketWriteOperation::suspend() {
    socket->_sendcor = _awaitingCoroutine;
}