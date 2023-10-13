#pragma once
#include <coroutine>
#include <utility>
#include <exception>
#include <tuple>

template<class T>
struct Generator;

template<class T>
struct promise_type_base {
    std::suspend_always initial_suspend() {return {};}
    std::suspend_never final_suspend() {return {};}
    Generator<T> get_return_object() {
        return Generator{std::coroutine_handle<promise_type_base<T>>::from_promise(*this)};
    }

    void unhandled_exception() { }

    void return_void() {

    }

    std::suspend_always yield_value(T value) {
        _value = value;
        _is_ready = true;
        return {};
    }
    T _value;
    bool _is_ready;
};  

template<class T>
struct Generator {
    class ExhaustedException: std::exception { };
    using promise_type = promise_type_base<T>;
    Generator(std::coroutine_handle<promise_type>handle):_handle(handle) {
        _handle.promise()._is_ready = false;
    }

    std::tuple<T*,bool> next() {
        if(_handle.done()) {
            return {nullptr,false};
        }
        if(has_next()) {
            _handle.promise()._is_ready = false;
            return {&_handle.promise()._value,true};
        }
        return {nullptr,false};
    }
    Generator(Generator& )=delete;
    Generator(Generator&& g):_handle(std::exchange(g._handle,{})) {

    }
    Generator& operator=(Generator&)=delete;
    
    bool has_next() {
        if(_handle.done()) { return false;}
        if(_handle.promise()._is_ready) {return true;}
        _handle.resume();
        if(_handle.done()) {return false;}
        return true;
    }
    ~Generator() {
        if(!_handle.done()) {
            _handle.destroy();
        }
    }
    std::coroutine_handle<promise_type>_handle;
};