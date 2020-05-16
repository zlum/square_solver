#include "quadreader.h"

#include "bignumber/bignumberbuilder.h"

#include <algorithm>
#include <exception>
#include <string>

using namespace std;

QuadReader::QuadReader(int argc, char* argv[],
                       shared_ptr<Buffer<QuadCoeffs>> outputBuf,
                       unique_ptr<BigNumberBuilder> coeffBuilder):
    _argc(argc),
    _argv(argv),
    _buf(outputBuf),
    _builder(move(coeffBuilder))
{
}

QuadReader::~QuadReader()
{
    // Join the worker thread to avoid worker() execution after this destructor
    join();
}

void QuadReader::stopLater()
{
    // Set work flag as false
    ProducerConsumer::stopLater();
    // Ware threads up to interrupt buffer operations
    _buf->notifyAll();
}

void QuadReader::worker()
{
    // Read 3 values per step. Skips argv[0] cause it is working directory
    // Returns if all arguments had been read
    size_t arg = 1;
    size_t pos = 0;

    while(arg < size_t(_argc))
    {
        try
        {
            _buf->emplace(readCoeffs(arg, pos), getWorkFlag());
        }
        catch(...)
        {
            break;
        }
    }
}

QuadCoeffs QuadReader::readCoeffs(size_t& arg, size_t& pos)
{
    size_t i = 0; // Counter of correctly read values
    QuadCoeffs coeffs;

    _builder->clear(); // Prepare builder

    // Read arguments until QuadCoeffs will be fulfilled
    while(i < 3)
    {
        string str(_argv[arg]); // Get next argument

        // Read string from currentPos to the end or invalid character
        size_t readNumber = _builder->appendStr(str, pos);

        if(_builder->isEmpty())
        {
            ++pos;
        }
        else
        {
            // Value had been read. Build number and go to the next
            pos += readNumber;
            coeffs.at(i) = _builder->build();
            ++i;
        }

        if(pos >= str.size())
        {
            // Current argument had been read to the end
            ++arg; // Skip current argument
            pos = 0; // Reset read postion
            _builder->clear(); // Reset builder (sign or dec point could be set)

            if((arg) >= size_t(_argc)) break; // Boundary check
        }
    }

    // Return coefficients if all of them had been read correctly
    if(i == 3)
    {
        return coeffs;
    }

    // FIXME: Throw my type
    // Couldn`t get correct input
    throw runtime_error("Bad input");
}
