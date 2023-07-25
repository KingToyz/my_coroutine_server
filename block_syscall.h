#pragma once
#include <coroutine>
#include <type_traits>
#include <errno.h>
#include <stdio.h>

template<class ReturnValue,class Caller>
class BlockSysCall {
    public:
        BlockSysCall():HaveSuspend_(false) {}

        bool await_ready() {
            return false;
        }

        bool await_suspend(std::coroutine_handle<> awaitingCoroutine) {
            _awaitingCoroutine = awaitingCoroutine;
            value_ = static_cast<Caller*>(this)->syscall();
            HaveSuspend_ = value_ == -1 && (errno == EAGAIN || errno == EWOULDBLOCK);
            if(HaveSuspend_) {
                static_cast<Caller*>(this)->suspend();
            }
            // } else {
            //     perror("error");
            //     printf("%d\n",errno);
            //     printf("%d\n",value_);
            // }
            return HaveSuspend_;
        }

        ReturnValue await_resume() {
            if (HaveSuspend_) {
                value_ = static_cast<Caller*>(this)->syscall();
            }
            return value_;
        }

    protected:
        ReturnValue value_;
        std::coroutine_handle<>_awaitingCoroutine;
        bool HaveSuspend_;
};