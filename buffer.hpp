#pragma once

#include <chrono>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>

// Thread-safe task queue
template<typename Task>
class Buffer
{
public:
    explicit Buffer() = default;
    ~Buffer() = default;

    // Thread-safe emplace to the queue
    void emplace(const Task& task)
    {
        std::unique_lock<std::mutex> lock(_queueMtx);
        _taskQueue.emplace(task);
        lock.unlock();
        _queueCondVar.notify_one();
    }

    // Thread-safe get front element and pop the queue
    // Block thread if there are no elements available
    Task getAndPop(const bool& workFlag) // FIXME: MB const& ?
    {
        std::unique_lock<std::mutex> lock(_queueMtx);

        while(_taskQueue.empty())
        {
            if(!workFlag)
            {
                throw std::runtime_error("Interrupted");
            }

            // Wake up every 500ms to check work flag
            _queueCondVar.wait_for(lock, std::chrono::milliseconds(500));
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
