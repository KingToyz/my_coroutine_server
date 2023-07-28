#include "socket.h"
#include "epoll_agent.h"
#include "task.h"
#include <memory>
#include <iostream>
#include <unordered_map>

task<> loop(Socket* s) {
    bool run = true;
    while (run) {
        run = co_await s->loop();
    }
}


task<> handle_connection(Socket* s) {
    auto t = loop(s);
    co_await t;
    delete s;
    t.handle_.destroy();
}

task<> accept(Socket& listen) {
    while (true) {
        // 这里get_return_object构造完成之后，在co_await中被挂起，走到promise_type的initial_suspend,代表第一次被挂起,挂起后走promise_type的await_ready()返回false，代表被挂起，再走task的await_suspend然后挂起，回到调用这个协程函数accept的下一步t.resume（30行）,然后走到listen.accept()内部
        auto socket = co_await listen.accept();
        if(socket) {
            auto t = handle_connection(socket);
            t.resume();
        }
    }
}

int main() {
    EpollAgent agent;
    Socket listen{"12345",&agent};
    auto t = accept(listen);
    t.resume();

    agent.Run();
}