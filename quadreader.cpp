#include "quadreader.h"

#include <algorithm>
#include <exception>

using namespace std;

QuadReader::QuadReader(int argc, char* argv[],
                       shared_ptr<Buffer<QuadCoeffs>> outputBuf):
    _argc(argc),
    _argv(argv),
    _buf(outputBuf)
{
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
    size_t i = 1;

    while(i + 2 < size_t(_argc))
    {
        try
        {
            _buf->emplace(readCoeffs(i, _argc, _argv), getWorkFlag());
        }
        catch(...)
        {
            break;
        }
    }
}

QuadCoeffs QuadReader::readCoeffs(size_t& current, int argc, char* argv[])
{
    size_t j = 0; // Counter of correctly read arguments
    QuadCoeffs coeffs;

    while(j < 3)
    {
        string str(argv[j + current]); // Get next argument

        // Remove junk symbols
        str.erase(remove_if(str.begin(), str.end(),
                            &QuadReader::isNotValidChar),
                  str.end());

        try
        {
            coeffs.at(j) = stoi(str); // Try to read int value
        }
        catch(...)
        {
            ++current; // Skip current argument
            if((current + j) >= size_t(argc)) break; // Boundary check
            continue;
        }

        ++j;
    }

    // Set current read marker to the next arguments
    current += 3;

    // Return coefficients if all of them had been read correctly
    if(j == 3)
    {
        return coeffs;
    }

    // Couldn`t get correct input
    throw runtime_error("Bad input");
}

bool QuadReader::isNotValidChar(char c)
{
    // Allows digits and '-' sign only
    return !bool(isdigit(c)) && (c != '-');
}
