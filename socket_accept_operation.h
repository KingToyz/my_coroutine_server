#pragma once
#include "block_syscall.h"
#include <coroutine>
class Socket;

class SocketAccpetOperation : public BlockSysCall<int,SocketAccpetOperation>{
    public:
        SocketAccpetOperation(Socket* s);
        ~SocketAccpetOperation();
        int syscall();

        void suspend();

    private:
       
        Socket* socket;
};