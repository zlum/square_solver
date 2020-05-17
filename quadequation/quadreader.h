#pragma once

#include "producerconsumer/producerconsumer.h"

#include "producerconsumer/buffer.hpp"
#include "quadtypes.h"

#include <memory>
#include <stdexcept>

// TODO: Replace with Coeff builder
class BigNumberBuilder;

// Reads coefficients from cli, stores to task and writes
// task to output buffer
class QuadReader:
        public ProducerConsumer
{
public:
    explicit QuadReader(int argc, char* argv[],
                        std::shared_ptr<Buffer<QuadCoeffs>> outputBuf,
                        std::unique_ptr<BigNumberBuilder> coeffBuilder);
    virtual ~QuadReader();

    // ProducerConsumer
    virtual void stopLater() override;

private:
    // ProducerConsumer
    virtual void worker() override;
    // Read coefficients of quadratic equation from cli arguments
    QuadCoeffs readCoeffs(size_t& arg, size_t& pos);

private:
    int _argc;
    char** _argv;
    std::shared_ptr<Buffer<QuadCoeffs>> _buf;
    std::unique_ptr<BigNumberBuilder> _builder;
};
