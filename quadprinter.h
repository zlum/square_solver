#pragma once

#include "producerconsumer.h"

#include "buffer.hpp"
#include "task.h"

#include <memory>

// Extract quadratic equations from input buffer and prints them to the screen
class QuadPrinter:
        public ProducerConsumer
{
public:
    explicit QuadPrinter(std::shared_ptr<Buffer<QuadEquation>> inputBuf);
    virtual ~QuadPrinter() = default;

    // ProducerConsumer
    virtual void stopLater() override;

private:
    // ProducerConsumer
    virtual void worker() override;

    // Prints quadratic equation in format:
    // (a b c) => (x1, x2)
    static void printQuadEquation(const QuadEquation& equation);

private:
    std::shared_ptr<Buffer<QuadEquation>> _buf;
};
