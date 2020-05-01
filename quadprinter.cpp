#include "quadprinter.h"

#include <cmath>
#include <iostream>

using namespace std;

QuadPrinter::QuadPrinter(std::shared_ptr<Buffer<QuadEquation>> inputBuf):
    _buf(inputBuf)
{
}

void QuadPrinter::stopLater()
{
    // Set work flag as false
    ProducerConsumer::stopLater();
    // Ware threads up to interrupt buffer operations
    _buf->notifyAll();
}

void QuadPrinter::worker()
{
    // Returns if stopLater() had been called and input buffer is empty
    while(true)
    {
        QuadEquation equation;

        try
        {
            equation = _buf->getAndPop(getWorkFlag());
        }
        catch(...)
        {
            break;
        }

        printQuadEquation(equation);
    }
}

void QuadPrinter::printQuadEquation(const QuadEquation& equation)
{
    // Print coefficients
    cout << '('
         << equation.coeffs.at(0) << ' '
         << equation.coeffs.at(1) << ' '
         << equation.coeffs.at(2) << ") => ";

    // Print roots
    // Using NaN as no root mark
    if(isnan(equation.roots.first))
    {
        cout << "no roots" << endl;
    }
    else
    if(isnan(equation.roots.second))
    {
        // One root
        cout << '('
             << equation.roots.first << ')'
             << endl;
    }
    else
    {
        // Two roots
        cout << '('
             << equation.roots.first << ", "
             << equation.roots.second << ')'
             << endl;
    }
}
