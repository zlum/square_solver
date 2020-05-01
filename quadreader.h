#pragma once

#include "producerconsumer.h"

#include "buffer.hpp"
#include "task.h"

#include <memory>

// Reads coefficients from cli, stores to task and writes
// task to output buffer
class QuadReader:
        public ProducerConsumer
{
public:
    explicit QuadReader(int argc, char* argv[],
                        std::shared_ptr<Buffer<QuadCoeffs>> outputBuf);
    virtual ~QuadReader() = default;

    // ProducerConsumer
    virtual void stopLater() override;

private:
    // ProducerConsumer
    virtual void worker() override;

    // Reads coefficients of quadratic equation from cli arguments
    static QuadCoeffs readCoeffs(size_t& current, int argc, char* argv[]);

    // Input validator
    static bool isNotValidChar(char c);

private:
    int _argc;
    char** _argv;
    std::shared_ptr<Buffer<QuadCoeffs>> _buf;
};
