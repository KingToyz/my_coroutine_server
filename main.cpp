#include "socket.h"
#include "epoll_agent.h"
#include "task.h"
#include <memory>
#include <iostream>

task<> loop(std::shared_ptr<Socket> s) {
    bool run = true;
    while (run) {
        run = co_await s->loop();
    }
}

task<> accept(Socket& listen) {
    while (true) {
        // 这里get_return_object构造完成之后，在co_await中被挂起，走到promise_type的initial_suspend,代表第一次被挂起,挂起后走promise_type的await_ready()返回false，代表被挂起，再走task的await_suspend然后挂起，回到调用这个协程函数accept的下一步t.resume,然后走到listen.accept()内部
        auto socket = co_await listen.accept();
        std::cout << "1.use_count" << socket.use_count() << std::endl;
        auto t = loop(socket);
        t.resume();
        std::cout << "2.use_count" << socket.use_count() << std::endl;
        // delete socket;
    }
}

int main() {
    EpollAgent agent;
    Socket listen{"12345",&agent};
    auto t = accept(listen);
    t.resume();

    agent.Run();
}