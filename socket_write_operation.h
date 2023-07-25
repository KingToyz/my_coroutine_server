#pragma once
#include <coroutine>
#include "block_syscall.h"

class Socket;
class SocketWriteOperation : public BlockSysCall<int,SocketWriteOperation>{
    public:
        SocketWriteOperation(void* buffer,Socket* s,int len);
        ~SocketWriteOperation();
        int syscall();

        void suspend();

    private:
        void* buffer;
        int len;
        Socket* socket;
};