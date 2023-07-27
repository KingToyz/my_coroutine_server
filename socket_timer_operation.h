#pragma once
#include <coroutine>
#include "block_syscall.h"
#include <string>

class Socket;
class SocketTimerOperation : public BlockSysCall<int,SocketTimerOperation>{
    public:
        SocketTimerOperation(Socket* s);
        ~SocketTimerOperation();
        int syscall();

        void suspend();

    private:
       Socket* s;
};