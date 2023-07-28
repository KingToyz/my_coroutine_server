#pragma once
#include <coroutine>
#include <iostream>

template<class T> struct task;
namespace mytask {

template <class T>
struct promise_type_base {
    std::coroutine_handle<> waiter;

    void return_value() {

    }
    std::suspend_always initial_suspend() { return {}; }
    struct final_awaiter {
        bool await_ready() { return false; }
        void await_resume() {}
        template <typename promise_type>
        void await_suspend(std::coroutine_handle<promise_type> me) {
                if (me.promise().waiter)
                    me.promise().waiter.resume();
                }
            };
    auto final_suspend() {
        return final_awaiter{};
    }

    void unhandled_exception() {
    }
};

template <typename T>
struct promise_type final : promise_type_base<T>
{
    T result;
    void return_value(T value){
        result = value;
    }
    task<T> get_return_object();
};

template <>
struct promise_type<void> final : promise_type_base<void>
{
    task<void> get_return_object();
};

}


template<class T=void>
struct task {
    using promise_type = mytask::promise_type<T>;
    task()
      : handle_{nullptr}
    {}
    task(std::coroutine_handle<promise_type> handle)
      : handle_{handle}
    {}
    ~task() {
        // std::cout << "handle_ destory" << std::endl;
        // handle_.destroy();
    }
    T await_resume();
    void await_suspend(std::coroutine_handle<> waiter) {
        handle_.promise().waiter = waiter;
        handle_.resume();
    }
    bool await_ready() {
        return false;
    }
    void resume() {
        handle_.resume();
    }
    std::coroutine_handle<promise_type> handle_;
};



template <class T>
T task<T>::await_resume() {
    return handle_.promise().result;
}

template <>
inline void task<void>::await_resume() {
}

namespace mytask {
template<class T>
task<T> promise_type<T>::get_return_object() {
    return task<T>{std::coroutine_handle<promise_type<T>>::from_promise(*this)};
}
inline task<void> promise_type<void>::get_return_object() {
    return task<void>{std::coroutine_handle<promise_type<void>>::from_promise(*this)};
}

}