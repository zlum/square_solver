#pragma once

#include "producerconsumer/producerconsumer.h"

#include "producerconsumer/buffer.hpp"
#include "quadtypes.h"

#include <memory>

// Extract quadratic equations from input buffer and prints them to the screen
class QuadPrinter:
        public ProducerConsumer
{
public:
    explicit QuadPrinter(std::shared_ptr<Buffer<QuadEquation>> inputBuf);
    virtual ~QuadPrinter();

    // ProducerConsumer
    virtual void stopLater() override;

private:
    // ProducerConsumer
    virtual void worker() override;

    // Print quadratic equation in format:
    // (a b c) => (x1, x2)
    static void printQuadEquation(const QuadEquation& equation);

private:
    std::shared_ptr<Buffer<QuadEquation>> _buf;
};
