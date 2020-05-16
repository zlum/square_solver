#include "quadsolver.h"

#include <cmath>
#include <limits>

using namespace std;
using namespace bigNumber;

QuadSolver::QuadSolver(shared_ptr<Buffer<QuadCoeffs>> inputBuf,
                       shared_ptr<Buffer<QuadEquation>> outputBuf):
    _inputBuf(inputBuf),
    _outputBuf(outputBuf)
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
        QuadEquation equation;
        QuadCoeffs coeffs;

        try
        {
            coeffs = _inputBuf->getAndPop(workFlag);
        }
        catch(...)
        {
            break;
        }

        equation.coeffs = coeffs;
        equation.roots = calcRoots(coeffs);

        try
        {
            _outputBuf->emplace(equation, workFlag);
        }
        catch(...)
        {
            break;
        }
    }
}

BigNumber QuadSolver::calcRoot(const BigNumber& sqrtDsc, bool isNegative,
                               const BigNumber& a, const BigNumber& b)
{
    // Create static object to use as integer literal
    static const BigNumber Big2{{2}, 0, Sign::positive, Status::normal};

    return (-b + (isNegative ? -sqrtDsc : sqrtDsc)) / (Big2 * a);
}

QuadRoots QuadSolver::calcRoots(const QuadCoeffs& coeffs)
{
    // Create static object to use as integer literal
    static const BigNumber Big4{{4}, 0, Sign::positive, Status::normal};
    static const BigNumber BigNaN{{}, 0, Sign::positive, Status::nan};

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
