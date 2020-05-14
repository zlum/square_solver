#include "bignumberbuilder.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>
#include <locale>
#include <stdexcept>

using namespace std;
using namespace bigNumber;

BigNumberBuilder::BigNumberBuilder():
    _fractPos(0),
    _sign(Sign::positive),
    _decimalPointFlag(false),
    _empty(true)
{
}

bool BigNumberBuilder::appendChar(char symbol)
{
    // Any appended digit changes empty status to false

    // Zero as first digit in integral part of number allowed but useless
    // Zero will be silently ignored
    if(_numIntPart.empty() && symbol == '0' && !_decimalPointFlag)
    {
        _empty = false;

        return true;
    }

    if(bool(isdigit(symbol)))
    {
        // TODO: Max size constexpr
        if(_numIntPart.size() >= SIZE_MAX - 1)
        {
            // FIXME: Throw my type
            throw out_of_range("Number out of range");
        }

        _numIntPart.emplace_back(symbol - '0');

        if(_decimalPointFlag)
        {
            ++_fractPos;
        }

        _empty = false;

        return true;
    }

    if(symbol == '-')
    {
        // Sign can be set only in the first position
        if(_numIntPart.empty() && !_decimalPointFlag)
        {
            _sign = Sign::negative;

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
    size_t i = 0;

    while(!_numIntPart.empty() && i < _fractPos)
    {
        if(_numIntPart.back() != 0)
        {
            break;
        }

        _numIntPart.pop_back();
        --_fractPos;
    }

    reverse(_numIntPart.begin(), _numIntPart.end());

    // NOTE: Fixed in append()
    // TODO: Use BigNumber::pop
    while(!_numIntPart.empty())
    {
        if(_numIntPart.back() != 0)
        {
            break;
        }

        _numIntPart.pop_back();
//        --_fractPos;
    }

    BigNumber num{_numIntPart, _fractPos, _decimalPointFlag, _sign, Status::normal};

    clear();

    return num;
}

void BigNumberBuilder::clear()
{
    _numIntPart.clear();
    _fractPos = 0;
    _decimalPointFlag = false;
    _sign = Sign::positive;
    _empty = true;
}

bool BigNumberBuilder::isEmpty() const
{
    return _empty;
}
