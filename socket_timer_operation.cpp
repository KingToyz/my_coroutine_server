#include "socket_timer_operation.h"

#include "const.h"
#include "sys/socket.h"
#include "socket.h"
#include "block_syscall.h"
#include "epoll_agent.h"
#include <iostream>
#include <unistd.h>

SocketTimerOperation::SocketTimerOperation(Socket* s):s(s) {
    s->agent->WatchWrite(s);
}

SocketTimerOperation::~SocketTimerOperation() {
    s->agent->UnWatchWrite(s);
}


int SocketTimerOperation::syscall() {
    uint64_t exp;
    return read(s->sockfd,&exp, sizeof(uint64_t));
}

void SocketTimerOperation::suspend() {
    s->_sendcor = _awaitingCoroutine;
}