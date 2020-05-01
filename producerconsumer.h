#pragma once

#include <thread>

// Abstract class with worker thread managment
class ProducerConsumer
{
public:
    explicit ProducerConsumer();
    virtual ~ProducerConsumer();

    // Starts worker() function in thread
    virtual void start();
    // Joins to worker thread
    virtual void join();

    // Returns reference to work flag
    const bool& getWorkFlag() const;
    // Set work flag as false
    void stopLater();

private:
    // Starts in _workerThread by start() call
    virtual void worker() = 0;

private:
    std::thread* _workerThread;
    bool _workFlag;
};
