#pragma once
#include <queue>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

// 异步日志队列
template <typename T>
class MyRpcLogQue
{
public:
    // 向队列中添加数据,多个worker线程会调用
    void Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        que_.push(data);
        // 通知等待的线程
        cond_.notify_one();
    }

    // 从队列中取数据，单个线程调用，向文件中写入日志
    T Pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (que_.empty())
        {
            // 等待队列中有数据,唤醒后继续判断
            cond_.wait(lock);
        }
        // 取出队列中的数据
        T data = que_.front();
        que_.pop();
        // 返回数据
        return data;
    }

    bool Empty()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return que_.empty();
    }

private:
    std::queue<T> que_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

