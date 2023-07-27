#pragma once
#include <arpa/inet.h>
#include <coroutine>
#include <string>
#include "task.h"
#include "socket_accept_operation.h"
#include "socket_recv_operation.h"
#include "socket_write_operation.h"
#include "socket_timer_operation.h"
#include <memory>
#include "parse.h"

class EpollAgent;
class Socket {
    public:
        ~Socket();
        Socket(std::string port,EpollAgent* agent);
        Socket(int fd,EpollAgent* agent);
        EpollAgent* agent;
        bool resumeRecv() {
            std::cout << "start to recv resume" << std::endl;
            if (!_recvcor)
                return false;
            std::cout << "recv resume" << std::endl;
            _recvcor.resume();
            return true;
        }

        bool resumeSend() {
            std::cout << "start to send resume" << std::endl;
            if (!_sendcor)
                return false;
            std::cout << "send resume" << std::endl;
            _sendcor.resume();
            return true;
        }
        int port;
        int sockfd;
        bool isclose;
        struct sockaddr_in sin;
        socklen_t socklen;
        std::coroutine_handle<>_recvcor;
        std::coroutine_handle<>_sendcor;
        int IOState;
        int NewIOState;
        Parser parser;

        task<std::shared_ptr<Socket>> accept();

        SocketRecvOperation recv(void* buffer, std::size_t len);
        SocketWriteOperation write(void* buffer, std::size_t len);
        SocketTimerOperation read();

        task<bool>loop();
};