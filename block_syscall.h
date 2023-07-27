#pragma once
#include <coroutine>
#include <type_traits>
#include <errno.h>
#include <stdio.h>
#include <iostream>
template<class ReturnValue,class Caller>
class BlockSysCall {
    public:
        BlockSysCall():HaveSuspend_(false) {}

        bool await_ready() {
            return false;
        }
        // awaitingCoroutine是本协程的handle，如果返回true，则挂起，如果是false，继续执行，返回handle则唤醒handle里的协程
        bool await_suspend(std::coroutine_handle<> awaitingCoroutine) {
            _awaitingCoroutine = awaitingCoroutine;
            value_ = static_cast<Caller*>(this)->syscall();
            perror("syscall\n");
            std::cout << errno << std::endl;
            std::cout << value_ << std::endl;
            HaveSuspend_ = value_ == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS);
            std::cout << "boolsusspend:" << HaveSuspend_ << std::endl;
            if(HaveSuspend_) {
                static_cast<Caller*>(this)->suspend();
            } else {
            
            }
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