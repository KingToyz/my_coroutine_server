#pragma once
#include <coroutine>
#include "block_syscall.h"


class Socket;
class SocketRecvOperation : public BlockSysCall<int,SocketRecvOperation>{
    public:
        SocketRecvOperation(void* buffer,Socket* s,int len);
        ~SocketRecvOperation();
        int syscall();

        void suspend();

    private:
        void* buffer;
        int len;
        Socket* socket;
};