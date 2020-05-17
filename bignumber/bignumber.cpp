#include "bignumber.h"

#include "numvector.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <locale>
#include <stdexcept>
#include <utility>

using namespace std;
using namespace bigNumber;
using namespace numVector;

BigNumber::BigNumber():
    _fractPos(0),
    _sign(Sign::positive),
    _status(Status::normal)
{
}

BigNumber::BigNumber(NumVector numIntPart, size_t fractPos,
                     Sign sign, Status status):
    _number(move(numIntPart)),
    _fractPos(fractPos),
    _sign(sign),
    _status(status)
{
}

bool BigNumber::isZero() const
{
    // Only normal numbers could be zero
    if(_status != Status::normal)
    {
        return false;
    }

    // Going from most significant digit
    // Always need only one step to get answer
    for(auto it = _number.rbegin(); it != _number.rend(); ++it)
    {
        if(*it != 0)
        {
            return false;
        }
    }

    return true;
}

Sign BigNumber::getSign() const
{
    return _sign;
}

Status BigNumber::getStatus() const
{
    return _status;
}

BigNumber BigNumber::sqrt() const
{
    // Ignore Inf and NaN numbers
    if(_status != Status::normal)
    {
        return *this;
    }

    // Result is NaN if number <= 0
    if(_sign == Sign::negative || isZero())
    {
        return BigNumber{_number, _fractPos, _sign, Status::nan};
    }

    // Newton's method
    // https://helloacm.com/coding-exercise-implement-integer-square-root-c-online-judge/
    static const BigNumber Big0_5{{5}, 1, Sign::positive, Status::normal};
    BigNumber dividend = *this;
    BigNumber val = *this;
    BigNumber last;

    // Number of approximations fixed in between 8 and 64
    // Based on number length because use of standart method of checking
    // precision by subtraction is inefficient
    constexpr size_t maxApproxNum = 64;
    constexpr size_t minApproxNum = 8;
    constexpr size_t approxStepPerNum = 5;

    size_t i = 0;
    size_t approxNum = min(size_t(maxApproxNum),
                           max(size_t(minApproxNum),
                               _number.size() * approxStepPerNum));

    do
    {
        ++i;
        last = val;
        val = (val + dividend / val) * Big0_5;
    }
    while(i < approxNum);

    return val;
}

BigNumber BigNumber::round() const
{
    // Only normal numbers can be rounded
    if(_status != Status::normal)
    {
        return *this;
    }

    // Rounding not required
    if(_fractPos <= _precision)
    {
        return *this;
    }

    // Determine rounding position and digit for rounding to the greater value
    size_t pos = _fractPos - _precision;
    size_t digitPos = pos - 1;
    size_t digit;

    if(digitPos < _number.size())
    {
        digit = _number.at(digitPos);
    }
    else
    {
        // All significant values are rounded
        return BigNumber{{}, 0, _sign, _status};
    }

    NumVector num;
    num.insert(num.begin(), _number.begin() + pos, _number.end());
    size_t numFractPos = _fractPos - (_number.size() - num.size());
    BigNumber res{num, numFractPos, _sign, _status};

    if(digit < 5)
    {
        // Rounding to the less value
        return res;
    }

    // Rounding to the greater value
    BigNumber rounder{{1}, numFractPos, _sign, _status};

    return res + rounder;
}

BigNumber BigNumber::operator -() const
{
    BigNumber num(*this);

    // Set inverted sign
    num._sign = (_sign == Sign::positive ? Sign::negative : Sign::positive);

    return num;
}

BigNumber BigNumber::operator +(const BigNumber& other) const
{
    // If left or right number is Inf or NaN then return it w/o changes
    if(_status != Status::normal)
    {
        return *this;
    }

    if(other._status != Status::normal)
    {
        return other;
    }

    // Sign managment and selecting of proper function
    // diff() has to be called with correct signs of numbers
    // sum() ignores number signs

    // -a + -b == -(a + b)
    if(_sign == Sign::negative && other._sign == Sign::negative)
    {
        return -sum(*this, other);
    }

    // -a + b == b - a
    if(_sign == Sign::negative)
    {
        return diff(other, -(*this));
    }

    // a + -b == a - b
    if(other._sign == Sign::negative)
    {
        return diff(*this, -other);
    }

    return sum(*this, other);
}

BigNumber BigNumber::operator -(const BigNumber& other) const
{
    // If left or right number is Inf or NaN then return it w/o changes
    if(_status != Status::normal)
    {
        return *this;
    }

    if(other._status != Status::normal)
    {
        return other;
    }

    // Sign managment and selecting of proper function
    // diff() has to be called with correct number signs
    // sum() ignores number signs

    // -a - -b == b - a
    if(_sign == Sign::negative && other._sign == Sign::negative)
    {
        return diff(-other, -(*this));
    }

    // -a - b == -(a + b)
    if(_sign == Sign::negative)
    {
        return -sum(*this, other);
    }

    // a - -b == a + b
    if(other._sign == Sign::negative)
    {
        return sum(*this, other);
    }

    return diff(*this, other);
}

BigNumber BigNumber::operator *(const BigNumber& other) const
{
    // If left or right number is Inf or NaN then return it w/o changes
    if(_status != Status::normal)
    {
        return *this;
    }

    if(other._status != Status::normal)
    {
        return other;
    }

    BigNumber num; // Result dummy
    Element carry = 0; // Increase number size or change next number digit

    num._sign = prodQuotSign(_sign, other._sign); // Determine sign
    num._fractPos = _fractPos + other._fractPos; // Determine decimal position

    // Number of zeroes in the begining of fractional part that can be skipped
    size_t skipZeroes = num._fractPos;

    // Multiply vectors. Additional digit will be written to (carry)
    num._number = prodOfVectors(_number, other._number,
                                    carry, skipZeroes);
    num._fractPos -= num._fractPos - skipZeroes;

    if(carry != 0)
    {
        num._number.emplace_back(carry);
    }

    return num;
}

BigNumber BigNumber::operator /(const BigNumber& other) const
{
    // If left or right number is Inf or NaN then return it w/o changes
    if(_status != Status::normal)
    {
        return *this;
    }

    if(other._status != Status::normal)
    {
        return other;
    }

    Sign sign = prodQuotSign(_sign, other._sign); // Determine sign

    // 0 / 0 == NaN, x / 0 == Inf
    // Sign is correct for division operation. Other data same as (this)
    if(other.isZero())
    {
        if(isZero())
        {
            return BigNumber{_number, _fractPos, sign, Status::nan};
        }

        return BigNumber{_number, _fractPos, sign, Status::inf};
    }

    BigNumber num;
    size_t decPos = 0;
    size_t lShift = 0;
    size_t rShift = 0;

    num._sign = sign;

    // Determine shift to shape correct column
    if(_fractPos > other._fractPos)
    {
        rShift = _fractPos - other._fractPos;
    }

    if(other._fractPos > _fractPos)
    {
        lShift = other._fractPos - _fractPos;
    }

    // Multiply vectors. Decimal point position will be written to (decPos)
    num._number = quotOfVectors(_number, other._number,
                                lShift, rShift, _precision, decPos);
    num._fractPos = decPos - lShift;

    return num;
}

bool BigNumber::operator >(const BigNumber& other) const
{
    // Zeroes are equal irrespective of sign
    if(isZero() && other.isZero())
    {
        return false;
    }

    // Positive numbers are greater than negative
    if(_sign == Sign::positive && other._sign == Sign::negative)
    {
        return true;
    }

    // Inf and NaN with same sign are not less nor greater
    if(_status != Status::normal)
    {
        return false;
    }

    return compareOfVectors(_number, _fractPos,
                            other._number, other._fractPos, greater<>());
}

bool BigNumber::operator <(const BigNumber& other) const
{
    // Zeroes are equal irrespective of sign
    if(isZero() && other.isZero())
    {
        return false;
    }

    // Negative numbers are less than positive
    if(_sign == Sign::negative && other._sign == Sign::positive)
    {
        return true;
    }

    // Inf and NaN with same sign are not less nor greater
    if(_status != Status::normal)
    {
        return false;
    }

    return compareOfVectors(_number, _fractPos,
                            other._number, other._fractPos, less<>());
}

bool BigNumber::operator ==(const BigNumber& other) const
{
    // Zeroes are equal irrespective of sign
    if(isZero() && other.isZero())
    {
        return true;
    }

    // Non-zero numbers with different signs are different
    if(_sign != other._sign)
    {
        return false;
    }

    // Same sign Inf are equal
    if(_status == Status::inf)
    {
        return true;
    }

    // Same sign NaN are different
    if(_status == Status::nan)
    {
        return false;
    }

    // If sizes or decimal point positions different then numbers are different
    // It is valid because insignificant digits not presented in vector
    if(_fractPos != other._fractPos || _number.size() != other._number.size())
    {
        return false;
    }

    // Comparison of digits from the most significant to the least
    size_t i = _number.size();

    while(i > 0)
    {
        --i;

        if(_number.at(i) != other._number.at(i))
        {
            return false;
        }
    }

    return true;
}

bool BigNumber::operator !=(const BigNumber& other) const
{
    return !this->operator ==(other);
}

ostream& operator <<(ostream& os, const BigNumber& num)
{
    // Output number in decimal format (ex. -123.456789)
    // Output 0, (-)Inf, (-)NaN in special cases
    if(num.isZero())
    {
        os << '0';

        return os;
    }

    if(num._sign == Sign::negative)
    {
        os << '-';
    }

    if(num.getStatus() == Status::inf)
    {
        os << "Inf";

        return os;
    }

    if(num.getStatus() == Status::nan)
    {
        os << "NaN";

        return os;
    }

    // Get current system locale to use correct decimal separator
    static const auto& loc = use_facet<numpunct<char>>(cout.getloc());
    size_t i;

    if(num._fractPos >= num._number.size())
    {
        // Output significant zero that not presented in vector
        os << '0';
        os << loc.decimal_point();
        i = num._fractPos;
    }
    else
    {
        i = num._number.size();
    }

    // Output little-endian number
    while(i > 0)
    {
        --i;

        if(i < num._number.size())
        {
            os << int(num._number.at(i));
        }
        else
        {
            // Output significant zero that not presented in vector
            os << '0';
        }

        // Output decimal separator if it is not the end of number
        if(i == num._fractPos && i != 0)
        {
            os << loc.decimal_point();
        }
    }

    return os;
}

BigNumber BigNumber::sum(const BigNumber& leftNum, const BigNumber& rightNum)
{
    // Sum two BigNumbers irrespective of sign or status
    BigNumber num;
    Element carry = 0;
    size_t lShift = 0;
    size_t rShift = 0;

    // Determine shift to shape correct column
    if(leftNum._fractPos > rightNum._fractPos)
    {
        rShift = leftNum._fractPos - rightNum._fractPos;
        num._fractPos = leftNum._fractPos;
    }
    else
    {
        lShift = rightNum._fractPos - leftNum._fractPos;
        num._fractPos = rightNum._fractPos;
    }

    // Number of zeroes in the begining of fractional part that can be skipped
    size_t skipZeroes = num._fractPos;

    // Sum vectors. Additional digit will be written to (carry)
    num._number = sumOfVectors(leftNum._number, rightNum._number,
                               carry, lShift, rShift, skipZeroes);

    num._fractPos -= num._fractPos - skipZeroes;

    if(carry != 0)
    {
        num._number.emplace_back(carry);
    }

    return num;
}

BigNumber BigNumber::diff(const BigNumber& leftNum, const BigNumber& rightNum)
{
    // Subtract right BigNumber from the left one
    BigNumber num;
    Element narrower = 0;
    size_t lShift = 0;
    size_t rShift = 0;

    // Determine shift to shape correct column
    if(leftNum._fractPos > rightNum._fractPos)
    {
        rShift = leftNum._fractPos - rightNum._fractPos;
        num._fractPos = leftNum._fractPos;
    }
    else
    {
        lShift = rightNum._fractPos - leftNum._fractPos;
        num._fractPos = rightNum._fractPos;
    }

    // Lesser vector must been subtracted from the greater one
    // Swap them and change result sign if necessary
    if(leftNum < rightNum)
    {
        num._number = diffOfVectors(rightNum._number, leftNum._number, narrower,
                                    rShift, lShift);
        num._sign = Sign::negative;
    }
    else
    {
        num._number = diffOfVectors(leftNum._number, rightNum._number, narrower,
                                    lShift, rShift);
    }

    return num;
}

Sign BigNumber::prodQuotSign(const Sign& lSign, const Sign& rSign)
{
    // Determine sign of a product or quotient
    if(lSign == rSign)
    {
        return Sign::positive;
    }

    return Sign::negative;
}
