#include "producerconsumer.h"

using namespace std;

ProducerConsumer::ProducerConsumer():
    _workerThread(nullptr),
    _workFlag(true)
{
}

ProducerConsumer::~ProducerConsumer()
{
    if(_workerThread->joinable())
    {
        _workerThread->join();
    }

    delete _workerThread;
}

void ProducerConsumer::start()
{
    _workerThread = new thread(&ProducerConsumer::worker, this);
}

void ProducerConsumer::join()
{
    _workerThread->join();
}

const bool& ProducerConsumer::getWorkFlag() const
{
    return _workFlag;
}

void ProducerConsumer::stopLater()
{
    _workFlag = false;
}
