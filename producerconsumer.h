#pragma once

#include <thread>

// Abstract class with worker thread managment
class ProducerConsumer
{
public:
    explicit ProducerConsumer();
    virtual ~ProducerConsumer();

    // Starts worker() function in thread
    void start();
    // Joins to the worker thread if it is joinable
    void join();
    // Set work flag as false
    virtual void stopLater();

    // Returns reference to work flag
    const bool& getWorkFlag() const;

private:
    // Starts in _workerThread by start() call
    virtual void worker() = 0;

private:
    std::thread* _workerThread;
    bool _workFlag;
};
