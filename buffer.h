#pragma once

#include "task.h"

#include <condition_variable>
#include <mutex>
#include <queue>

class Buffer
{
public:
    explicit Buffer();
    ~Buffer() = default;

    // Thread-safe push to the queue
    void push(const Task& task)
    {
        std::unique_lock<std::mutex> lock(_queueMtx);
        _taskQueue.emplace(task);
        lock.unlock();
        _queueCondVar.notify_one();
    }

    // Thread-safe get front element and pop the queue
    // Block thread if there are no elements available
    Task getAndPop()
    {
        std::unique_lock<std::mutex> lock(_queueMtx);

        while(_taskQueue.empty())
        {
            _queueCondVar.wait(lock);
        }

        Task val = _taskQueue.front();
        _taskQueue.pop();
        return val;
    }

private:
    std::queue<Task> _taskQueue;
    std::mutex _queueMtx;
    std::condition_variable _queueCondVar;
};
