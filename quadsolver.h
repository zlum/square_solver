#pragma once

#include "producerconsumer.h"

#include "buffer.hpp"
#include "task.h"

#include <memory>
#include <utility>

// Reads coefficients from input buffer, calc roots and writes
// solution to output buffer
class QuadSolver:
        public ProducerConsumer
{
public:
    explicit QuadSolver(std::shared_ptr<Buffer<QuadCoeffs>> inputBuf,
                        std::shared_ptr<Buffer<QuadEquation>> outputBuf);
    virtual ~QuadSolver();

    virtual void stopLater() override;

private:
    // ProducerConsumer
    virtual void worker() override;

    // Helper function to calculate one root
    static BigNumber calcRoot(const BigNumber& sqrtDsc, bool isNegative,
                              const BigNumber& a, const BigNumber& b);

    // Calculates roots of quadratic equation
    // Using NaN as no root mark
    static QuadRoots calcRoots(const QuadCoeffs& coeffs);

private:
    std::shared_ptr<Buffer<QuadCoeffs>> _inputBuf;
    std::shared_ptr<Buffer<QuadEquation>> _outputBuf;
};
