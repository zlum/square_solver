#include "quadsolver.h"

#include <cmath>
#include <limits>

using namespace std;

QuadSolver::QuadSolver(shared_ptr<Buffer<QuadCoeffs>> inputBuf,
                       shared_ptr<Buffer<QuadEquation>> outputBuf):
    _inputBuf(inputBuf),
    _outputBuf(outputBuf)
{
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

double QuadSolver::calcRoot(double sqrtDsc, bool isNegative, int a, int b)
{
    return (-b + (isNegative ? -sqrtDsc : sqrtDsc)) / (2.0 * a);
}

pair<double, double> QuadSolver::calcRoots(const QuadCoeffs& coeffs)
{
    int a = coeffs.at(0);
    int b = coeffs.at(1);
    int c = coeffs.at(2);

    // Check if it is a quadratic equation
    if(a == 0)
    {
        if(b == 0)
        {
            // Both x coefficients are zero
            return {numeric_limits<double>::quiet_NaN(),
                    numeric_limits<double>::quiet_NaN()};
        }

        // Calc as simple bx + c = 0 equation
        return {double(-c) / double(b),
                numeric_limits<double>::quiet_NaN()};
    }

    int discriminant = (b * b) - (4 * a * c);

    // Determines root count by discriminant sign
    if(discriminant > 0)
    {
        // Two roots
        double sqrtDsc = sqrt(discriminant);

        return {calcRoot(sqrtDsc, true, a, b),
                calcRoot(sqrtDsc, false, a, b)};
    }

    if(discriminant == 0)
    {
        // One root
        double sqrtDsc = sqrt(discriminant);

        return {calcRoot(sqrtDsc, false, a, b),
                numeric_limits<double>::quiet_NaN()};
    }

    // No roots
    return {numeric_limits<double>::quiet_NaN(),
            numeric_limits<double>::quiet_NaN()};
}
