#pragma once

#include <memory>
#include <thread>

// Abstract class with worker thread managment
class ProducerConsumer
{
public:
    explicit ProducerConsumer();
    virtual ~ProducerConsumer();

    // Start worker() function in thread
    void start();
    // Join to the worker thread if it is joinable
    void join();
    // Set work flag as false
    virtual void stopLater();

    // Return reference to work flag
    const bool& getWorkFlag() const;

private:
    // Run in _workerThread by start() call
    virtual void worker() = 0;

private:
    std::unique_ptr<std::thread> _workerThread;
    bool _workFlag;
};
