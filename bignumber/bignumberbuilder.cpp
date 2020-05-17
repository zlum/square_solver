#include "bignumberbuilder.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>
#include <locale>
#include <stdexcept>

using namespace bigNumber;
using namespace numVector;
using namespace std;

BigNumberBuilder::BigNumberBuilder():
    _decimalPointFlag(false),
    _empty(true)
{
}

bool BigNumberBuilder::appendChar(char symbol)
{
    // Any appended digit changes empty status to false

    // Zero as first digit in integral part of number allowed but useless
    // Zero will be silently ignored
    if(_dummyNum._number.empty() && symbol == '0' && !_decimalPointFlag)
    {
        _empty = false;

        return true;
    }

    if(bool(isdigit(symbol)))
    {
        // TODO: Max size constexpr
        if(_dummyNum._number.size() >= SIZE_MAX - 1)
        {
            // FIXME: Throw my type
            throw out_of_range("Number out of range");
        }

        _dummyNum._number.emplace_back(symbol - '0');

        if(_decimalPointFlag)
        {
            ++_dummyNum._fractPos;
        }

        _empty = false;

        return true;
    }

    if(symbol == '-')
    {
        // Sign can be set only in the first position
        if(_dummyNum._number.empty() && !_decimalPointFlag)
        {
            _dummyNum._sign = Sign::negative;

            return true;
        }

        return false;
    }

    if(symbol == ',' || symbol == '.')
    {
        // Decimal point can be set only once
        if(!_decimalPointFlag)
        {
            _decimalPointFlag = true;

            return true;
        }

        return false;
    }

    return false;
}

size_t BigNumberBuilder::appendStr(const string& str, size_t pos)
{
    size_t i = pos;

    while(i < str.size())
    {
        if(!appendChar(str.at(i)))
        {
            break;
        }

        ++i;
    }

    // Position of symbol that couldn't been read or the end of string
    return i;
}

BigNumber BigNumberBuilder::build()
{
    // Remove insignificant zeroes from the end of number
    // and adjust decimal point position
    _dummyNum._fractPos -= popZeroes(_dummyNum._number, _dummyNum._fractPos);

    // Reverse number to little-endian order used by BigNumber
    reverse(_dummyNum._number.begin(), _dummyNum._number.end());

    // Remove insignificant zeroes from another end of number
    popZeroes(_dummyNum._number, _dummyNum._number.size());

    // Copy result to return
    BigNumber num = _dummyNum;

    // Reset own data to default
    clear();

    return num;
}

void BigNumberBuilder::clear()
{
    // Reset own data to default
    _dummyNum._number.clear();
    _dummyNum._fractPos = 0;
    _dummyNum._sign = Sign::positive;
    _dummyNum._status = Status::normal;
    _decimalPointFlag = false;
    _empty = true;
}

bool BigNumberBuilder::isEmpty() const
{
    return _empty;
}
