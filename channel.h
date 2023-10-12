#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <coroutine>
#include <list>
#include <atomic>
#include <tuple>
#include <iostream>

template<class T>
struct Channel;

template<class T>
struct WriteAwaiter {
    Channel<T>* channel;
    T _value;
    std::coroutine_handle<> handle;

    explicit WriteAwaiter(Channel<T>* channel,T value):channel(channel),_value(value){}

    bool await_ready() {
        return false;
    }

    // 挂起的时候做的事
    void await_suspend(std::coroutine_handle<> coroutine_handle) {
        this->handle = coroutine_handle;
        channel->push_writer(this);
    }

    // 唤醒的时候做的事
    void await_resume() {
        channel->check_closed();
    }

    void resume() {
        handle.resume();
    }
};

template<class T>
struct ReadAwaiter {
    Channel<T>* channel;
    T _value;
    T* pvalue;
    bool* pclose;
    std::coroutine_handle<> handle;

    explicit ReadAwaiter(Channel<T>* channel):channel(channel){}

    bool await_ready() {
        return false;
    }

    // 挂起的时候做的事
    void await_suspend(std::coroutine_handle<> coroutine_handle) {
        this->handle = coroutine_handle;
        channel->push_reader(this);
    }

    // 唤醒的时候做的事
    void await_resume() {
       
    }

    void resume(T value) {
        _value = value;
        if(pvalue) {
            *pvalue = value;
        }
        resume();
    }

    void resume() {
        if(channel->is_close()) {
            *pclose = true;
        }
        handle.resume();
    }

};

template<class T>
struct Channel {
    struct ChannelClosedException : std::exception {
    const char *what() const noexcept override {
      return "Channel is closed.";
    }
  };

  explicit Channel(int cap = 0):buffer_capacity(cap) {
    _is_closed.store(false,std::memory_order_relaxed);
  }

  void close() {
    _is_closed.store(true,std::memory_order_relaxed);
    clean_up();
  }

  void check_closed() {
    // 如果已经关闭，则抛出异常
    if (_is_closed.load(std::memory_order_relaxed)) {
      throw ChannelClosedException();
    }
  }

  WriteAwaiter<T> write(T value) {
    check_closed();
    return WriteAwaiter<T>(this,value);
  }

  auto operator<< (T value) {
    return write(value);
  }

  auto read(T& value,bool& closed) {
    
    auto reader =  ReadAwaiter<T>(this);
    reader.pvalue = &value;
    reader.pclose = &closed;
    return reader;
  }


  void push_writer(WriteAwaiter<T>* write_awaiter) {
    std::unique_lock l(channel_lock);
    
    check_closed();
    if(reader_list.size() != 0) {
        auto* reader = reader_list.front();
        reader_list.pop_front();
        l.unlock();
        reader->resume(write_awaiter->_value);
        write_awaiter->resume();
        return;
    }

    if(buffer.size() != buffer_capacity) {
        buffer.push(write_awaiter->_value);
        l.unlock();
        write_awaiter->resume();
        return;
    }
    l.unlock();
    writer_list.push_back(write_awaiter);

  }

  void push_reader(ReadAwaiter<T>* read_awaiter) {
    std::unique_lock l(channel_lock);
    
    if(is_close()) {
        l.unlock();
        read_awaiter->resume();
        return;
    }
    if(buffer.size() != 0) {
        auto val = buffer.front();
        buffer.pop();
        
        l.unlock();
        read_awaiter->resume(val);
        return;
    }
    if(writer_list.size() != 0) {
        auto* writer = writer_list.front();
        writer_list.pop_front();
        
        l.unlock();
        read_awaiter->resume(writer->_value);
        writer->resume();
        return;
    }
    
    l.unlock();
    reader_list.push_back(read_awaiter);
  }

  bool is_close() {
    return _is_closed.load(std::memory_order_relaxed);
  }

  private:
  // buffer 的容量
  int buffer_capacity;
  std::queue<T> buffer;
  // buffer 已满时，新来的写入者需要挂起保存在这里等待恢复
  std::list<WriteAwaiter<T> *> writer_list;
  // buffer 为空时，新来的读取者需要挂起保存在这里等待恢复
  std::list<ReadAwaiter<T> *> reader_list;

  std::atomic<bool> _is_closed;

  std::mutex channel_lock;
  std::condition_variable channel_condition;

  void clean_up() {
    std::lock_guard lock(channel_lock);

    // 需要对已经挂起等待的协程予以恢复执行
    for (auto writer : writer_list) {
      writer->resume();
    }
    writer_list.clear();

    for (auto reader : reader_list) {
      reader->resume();
    }
    reader_list.clear();

    
    // 清空 buffer
    decltype(buffer) empty_buffer;
    std::swap(buffer, empty_buffer);
  }
};


