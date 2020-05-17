#include "quadsolver.h"

#include <cmath>
#include <limits>
#include <utility>

using namespace bigNumber;
using namespace std;

QuadSolver::QuadSolver(shared_ptr<Buffer<unique_ptr<QuadCoeffs>>> inputBuf,
                       shared_ptr<Buffer<unique_ptr<QuadEquation>>> outputBuf):
    _inputBuf(move(inputBuf)),
    _outputBuf(move(outputBuf))
{
}

QuadSolver::~QuadSolver()
{
    // Join the worker thread to avoid worker() execution after this destructor
    join();
}

void QuadSolver::stopLater()
{
    // Set work flag as false
    ProducerConsumer::stopLater();
    // Ware threads up to interrupt buffer operations
    _inputBuf->notifyAll();
    _outputBuf->notifyAll();
}

void QuadSolver::worker()
{
    // Returns if stopLater() had been called and input buffer is empty
    const bool& workFlag = getWorkFlag();

    while(true)
    {
        unique_ptr<QuadCoeffs> coeffs = _inputBuf->getAndPop(workFlag);

        if(coeffs == nullptr)
        {
            // Interrupt called
            break;
        }

        // Create equation
        auto equation = make_unique<QuadEquation>();

        equation->coeffs = move(*coeffs);
        equation->roots = calcRoots(equation->coeffs);

        if(!_outputBuf->emplace(move(equation), workFlag))
        {
            // Interrupt called
            break;
        }
    }
}

BigNumber QuadSolver::calcRoot(const BigNumber& sqrtDsc, bool isNegative,
                               const BigNumber& a, const BigNumber& b)
{
    // Create static object to use as integer literal
    static const BigNumber Big2{"2"};

    return (-b + (isNegative ? -sqrtDsc : sqrtDsc)) / (Big2 * a);
}

QuadRoots QuadSolver::calcRoots(const QuadCoeffs& coeffs)
{
    // Create static object to use as integer literal
    static const BigNumber Big4{"4"};
    static const BigNumber BigNaN{Sign::positive, Status::nan};

    // Get aliases for convenience
    const BigNumber& a = coeffs.at(0);
    const BigNumber& b = coeffs.at(1);
    const BigNumber& c = coeffs.at(2);

    // Check if it is a quadratic equation
    if(a.isZero())
    {
        if(b.isZero())
        {
            // Both x coefficients are zero
            return {BigNaN, BigNaN};
        }

        // Calc as simple bx + c = 0 equation
        return {b / -c, BigNaN};
    }

    BigNumber discriminant = (b * b) - (Big4 * a * c);

    // Determines root count by discriminant sign
    if(!discriminant.isZero() && discriminant.getSign() == Sign::positive)
    {
        // Two roots
        BigNumber sqrtDsc = discriminant.sqrt();

        return {calcRoot(sqrtDsc, true, a, b),
                calcRoot(sqrtDsc, false, a, b)};
    }

    if(discriminant.isZero())
    {
        // One root
        return {calcRoot(discriminant.sqrt(), false, a, b),
                BigNaN};
    }

    // No roots
    return {BigNaN, BigNaN};
}
