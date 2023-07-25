#pragma once
#include <arpa/inet.h>
#include <coroutine>
#include <string>
#include "task.h"
#include "socket_accept_operation.h"
#include "socket_recv_operation.h"
#include "socket_write_operation.h"
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
            if (!_recvcor)
                return false;
            _recvcor.resume();
            return true;
        }

        bool resumeSend() {
            if (!_sendcor)
                return false;
            _sendcor.resume();
            return true;
        }
        int port;
        int sockfd;
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


        task<bool>loop();
};