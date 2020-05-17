#pragma once

#include "producerconsumer/producerconsumer.h"

#include "producerconsumer/buffer.hpp"
#include "quadtypes.h"

#include <memory>

// Reads coefficients from input buffer, calc roots and writes
// solution to output buffer
class QuadSolver:
        public ProducerConsumer
{
public:
    explicit QuadSolver(std::shared_ptr<Buffer<std::unique_ptr<QuadCoeffs>>>
                            inputBuf,
                        std::shared_ptr<Buffer<std::unique_ptr<QuadEquation>>>
                            outputBuf);
    virtual ~QuadSolver();

    virtual void stopLater() override;

private:
    // ProducerConsumer
    virtual void worker() override;

    // Helper function to calculate one root
    static BigNumber calcRoot(const BigNumber& sqrtDsc, bool isNegative,
                              const BigNumber& a, const BigNumber& b);

    // Calculate roots of quadratic equation
    // Using NaN as no root mark
    static QuadRoots calcRoots(const QuadCoeffs& coeffs);

private:
    std::shared_ptr<Buffer<std::unique_ptr<QuadCoeffs>>> _inputBuf;
    std::shared_ptr<Buffer<std::unique_ptr<QuadEquation>>> _outputBuf;
};
