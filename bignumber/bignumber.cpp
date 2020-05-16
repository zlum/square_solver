#include "bignumber.h"

#include <algorithm>
#include <limits>
#include <locale>
#include <stdexcept>
#include <utility>

using namespace std;
using namespace bigNumber;

BigNumber::BigNumber():
    _fractPos(0),
    _sign(Sign::positive),
    _status(Status::normal)
{
}

BigNumber::BigNumber(vector<uint8_t> numIntPart,
                     size_t fractPos,
                     Sign sign,
                     Status status):
    _numIntPart(move(numIntPart)),
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
    for(auto it = _numIntPart.rbegin(); it != _numIntPart.rend(); ++it)
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
        return BigNumber{_numIntPart, _fractPos, _sign, Status::nan};
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
                               _numIntPart.size() * approxStepPerNum));

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
    if(_status != Status::normal)
    {
        return *this;
    }

    if(_fractPos <= _precision)
    {
        return *this;
    }

    size_t pos = _fractPos - _precision;
    size_t digitPos = pos - 1;
    size_t digit;

    if(digitPos < _numIntPart.size())
    {
        digit = _numIntPart.at(digitPos);
    }
    else
    {
        return BigNumber{{}, 0, _sign, _status};
    }

    std::vector<uint8_t> num;
    num.insert(num.begin(), _numIntPart.begin() + pos, _numIntPart.end());
    size_t numFractPos = _fractPos - (_numIntPart.size() - num.size());
    BigNumber res{num, numFractPos, _sign, _status};

    if(digit < 5)
    {
        return res;
    }

    BigNumber rounder{{1}, numFractPos, _sign, _status};

    return res + rounder;
}

BigNumber BigNumber::operator -() const
{
    BigNumber num(*this);

    // Set inverted sign
    num.setSign(_sign == Sign::positive ? Sign::negative : Sign::positive);

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
    uint8_t carry = 0; // Increase number size or change next number digit

    num._sign = prodQuotSign(_sign, other._sign); // Determine sign
    num._fractPos = _fractPos + other._fractPos; // Determine decimal position

    // Multiply vectors. Additional digit will be written to (carry)
    vector<uint8_t> prod = prodOfVectors(_numIntPart, other._numIntPart, carry);

    // NOTE: Zero cleaner
    size_t zeroPos = trackZeroes(prod, 0);
    size_t driftPos = min(zeroPos, num._fractPos);

    num._numIntPart.insert(num._numIntPart.begin(), prod.begin() + driftPos, prod.end());
    num._fractPos -= driftPos;

//    num._numIntPart = prodOfVectors(_numIntPart, other._numIntPart, carry);

    if(carry != 0)
    {
        num._numIntPart.emplace_back(carry);
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
            return BigNumber{_numIntPart, _fractPos, sign, Status::nan};
        }

        return BigNumber{_numIntPart, _fractPos, sign, Status::inf};
    }

    // TODO: Comments
    BigNumber num;
    uint8_t narrower = 0;
    size_t lUp = 0;
    size_t rUp = 0;

    num._sign = sign;

    if(_fractPos > other._fractPos)
    {
        rUp = _fractPos - other._fractPos;
    }

    if(other._fractPos > _fractPos)
    {
        lUp = other._fractPos - _fractPos;
    }

    num._numIntPart = quotOfVectors(_numIntPart, other._numIntPart, 0, rUp, lUp, narrower);
    num._fractPos = narrower - lUp; //abs(int64_t(_fractPos - other._fractPos)); // TODO: Rename

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

    // FIXME: Copypaste and conversion
    if(int64_t(_numIntPart.size() - _fractPos) < int64_t(other._numIntPart.size() - other._fractPos))
    {
        return false;
    }

    if(int64_t(_numIntPart.size() - _fractPos) > int64_t(other._numIntPart.size() - other._fractPos))
    {
        return true;
    }

    return isVectorGreater(_numIntPart, _fractPos,
                           other._numIntPart, other._fractPos);
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

    if(int64_t(_numIntPart.size() - _fractPos) > int64_t(other._numIntPart.size() - other._fractPos))
    {
        return false;
    }

    if(int64_t(_numIntPart.size() - _fractPos) < int64_t(other._numIntPart.size() - other._fractPos))
    {
        return true;
    }

    return isVectorLess(_numIntPart, _fractPos,
                        other._numIntPart, other._fractPos);
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
    if(_fractPos != other._fractPos ||
       _numIntPart.size() != other._numIntPart.size())
    {
        return false;
    }

    // Comparison of digits from the most significant to the least
    size_t i = _numIntPart.size();

    while(i > 0)
    {
        --i;

        if(_numIntPart.at(i) != other._numIntPart.at(i))
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

    if(num._fractPos >= num._numIntPart.size())
    {
        // Output significant zero that not presented in vector
        os << '0';
        os << loc.decimal_point();
        i = num._fractPos;
    }
    else
    {
        i = num._numIntPart.size();
    }

    // Output little-endian number
    while(i > 0)
    {
        --i;

        if(i < num._numIntPart.size())
        {
            os << int(num._numIntPart.at(i));
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
    uint8_t carry = 0;
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

    // Sum vectors. Additional digit will be written to (carry)
    vector<uint8_t> sum = sumOfVectors(leftNum._numIntPart,
                                       rightNum._numIntPart, carry,
                                       lShift, rShift);

    // NOTE: Zero cleaner
    size_t zeroPos = trackZeroes(sum, 0);
    size_t driftPos = min(zeroPos, num._fractPos);

    num._numIntPart.insert(num._numIntPart.begin(), sum.begin() + driftPos, sum.end());
    num._fractPos -= driftPos;

    if(carry != 0)
    {
        num._numIntPart.emplace_back(carry);
    }

    return num;
}

BigNumber BigNumber::diff(const BigNumber& leftNum, const BigNumber& rightNum)
{
    BigNumber num;
    uint8_t narrower = 0;
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
        num._numIntPart = diffOfVectors(rightNum._numIntPart,
                                        leftNum._numIntPart, narrower,
                                        rShift, lShift);
        num._sign = Sign::negative;
    }
    else
    {
        num._numIntPart = diffOfVectors(leftNum._numIntPart,
                                        rightNum._numIntPart, narrower,
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

void BigNumber::setSign(Sign sign)
{
    _sign = sign;
}

// NOTE: Dupe from builder
void BigNumber::popZeroes(vector<uint8_t>& vec)
{
    // Remove zeroes from the back of vector
    while(!vec.empty())
    {
        if(vec.back() != 0)
        {
            break;
        }

        vec.pop_back();
    }
}

vector<uint8_t> BigNumber::sumOfVectors(const vector<uint8_t>& lNum,
                                        const vector<uint8_t>& rNum,
                                        uint8_t& carry,
                                        size_t lShift,
                                        size_t rShift)
{
    // Sum vectors. Additional digit will be written to (carry)
    vector<uint8_t> sumNum;
    size_t maxSize = max(lNum.size() + lShift, rNum.size() + rShift);

    // Reserve maximum possible capacity
    sumNum.reserve(maxSize + 1);

    for(size_t i = 0; i < maxSize; ++i)
    {
        uint8_t sum = 0;
        uint8_t lval = 0;
        uint8_t rval = 0;

        if(i >= lShift && i < lNum.size() + lShift)
        {
            lval = lNum.at(i - lShift);
        }

        if(i >= rShift && i < rNum.size() + rShift)
        {
            rval = rNum.at(i - rShift);
        }

        sum = lval + rval + carry;

        if(sum > 9)
        {
            carry = 1;
            sum -= 10;
        }
        else
        {
            carry = 0;
        }

        sumNum.emplace_back(sum);
    }

    return sumNum;
}

vector<uint8_t> BigNumber::diffOfVectors(const vector<uint8_t>& lNum,
                                         const vector<uint8_t>& rNum,
                                         uint8_t& carry,
                                         size_t lShift,
                                         size_t rShift)
{
    // Subtract vectors. Additional digit will be written to (carry)
    vector<uint8_t> diffNum; // Result dummy
    size_t maxSize = max(lNum.size() + lShift, rNum.size() + rShift);

    // Reserve maximum possible capacity
    diffNum.reserve(maxSize);

    for(size_t i = 0; i < maxSize; ++i)
    {
        int diff = 0;
        uint8_t lval = 0;
        uint8_t rval = 0;

        if(i >= lShift && i < lNum.size() + lShift)
        {
            lval = lNum.at(i - lShift);
        }

        if(i >= rShift && i < rNum.size() + rShift)
        {
            rval = rNum.at(i - rShift);
        }

        diff = lval - rval - carry;

        if(diff < 0)
        {
            carry = 1;

            if(i == maxSize - 1)
            {
                diff = abs(diff);
            }
            else
            {
                diff += 10;
            }
        }
        else
        {
            carry = 0;
        }

        diffNum.emplace_back(diff);
    }

    // Remove zeroes from the back of vector
    popZeroes(diffNum);

    return diffNum;
}

void BigNumber::quotHelperSubtract(vector<uint8_t>& lNum,
                                   const vector<uint8_t>& rNum,
                                   size_t rShift)
{
    // Subtract right vector from the left inplace
    // Vactors have to be prepared in quotOfVectors() function
    uint8_t carry = 0;

    size_t i = lNum.size();

    while(i > 0)
    {
        int diff = 0;
        uint8_t lval = 0;
        uint8_t rval = 0;

        --i;

        if(i < lNum.size())
        {
            lval = lNum.at(i);
        }

        if(i >= rShift && i < rNum.size() + rShift)
        {
            rval = rNum.at(i - rShift);
        }
        else
        {
            if(carry == 0)
            {
                break;
            }
        }

        diff = lval - rval - carry;

        if(diff < 0)
        {
            carry = 1;

            if(i == 0)
            {
                diff = abs(diff);
            }
            else
            {
                diff += 10;
            }
        }
        else
        {
            carry = 0;
        }

        lNum.at(i) = diff;
    }
}

vector<uint8_t> BigNumber::prodHelperMultiply(const vector<uint8_t>& lNum,
                                              uint8_t multiplier)
{
    // Multiply vector by (multiplier)
    vector<uint8_t> prodVec; // Result dummy
    uint8_t carry = 0;

    // Reserve maximum possible capacity
    prodVec.reserve(lNum.size() + 1);

    for(const auto& next : lNum)
    {
        uint8_t prod = (next * multiplier) + carry;

        if(prod > 9)
        {
            carry = prod / 10;
            prod -= carry * 10;
        }
        else
        {
            carry = 0;
        }

        prodVec.emplace_back(prod);
    }

    if(carry != 0)
    {
        prodVec.emplace_back(carry);
    }

    return prodVec;
}

vector<uint8_t> BigNumber::prodOfVectors(const vector<uint8_t>& lNum,
                                         const vector<uint8_t>& rNum,
                                         uint8_t& carry)
{
    vector<uint8_t> prodNum; // Result dummy
    size_t rShift = 0; // Shift of right number in sum to shape correct column

    // Reserve maximum possible capacity
    prodNum.reserve(lNum.size() + rNum.size());

    for(const auto& next : rNum)
    {
        prodNum = sumOfVectors(prodNum, prodHelperMultiply(lNum, next),
                               carry, 0, rShift);

        if(carry != 0)
        {
            prodNum.emplace_back(carry);
            carry = 0;
        }

        ++rShift;
    }

    return prodNum;
}

size_t BigNumber::trackZeroes(const vector<uint8_t>& vec, size_t pos)
{
    for(size_t i = pos; i < vec.size(); ++i)
    {
        if(vec.at(i) != 0)
        {
            return i;
        }
    }

    return vec.size();
}

vector<uint8_t> BigNumber::quotOfVectors(const vector<uint8_t>& lNum,
                                         const vector<uint8_t>& rNum,
                                         size_t lUp,
                                         size_t rUp,
                                         size_t precision,
                                         uint8_t& narrower) // TODO: RM
{
    vector<uint8_t> quotNum;
    size_t lShift = 0;
    size_t rShift = 0;

    quotNum.reserve(lNum.size());

    if(rNum.size() > lNum.size())
    {
        rShift = rNum.size() - lNum.size(); // TODO: RM
    }
    else
    {
        lShift = lNum.size() - rNum.size();
    }

    vector<uint8_t> lNumPart(lUp);
    vector<uint8_t> rNumPart(rUp);

    int64_t prealloCounter = lShift - rUp;
    lShift = max(int64_t(0), prealloCounter);

    // Insert in reversed order
    // TODO: fix initial digit count with rUp
    lNumPart.insert(lNumPart.begin(), lNum.rbegin(), lNum.rend() - lShift);
    rNumPart.insert(rNumPart.begin(), rNum.rbegin(), rNum.rend());

//    if(prealloCounter < 0)
//    {
//        lNumPart.resize(lNumPart.size() + abs(prealloCounter));
//        narrower += abs(prealloCounter);
//    }

    if(lNumPart.size() < rNumPart.size())
    {
        narrower += rNumPart.size() - lNumPart.size();
        lNumPart.resize(rNumPart.size());
    }

    size_t i = lNumPart.size();

    while(true) // FIXME: ouf
    {
        if(isVectorLess(lNumPart, rNumPart))
        {
            uint8_t digit;

            ++i; // FIXME: Dupe

            if(!quotNum.empty())
            {
                quotNum.emplace_back(0);
            }

            if(i <= lNum.size())
            {
                digit = lNum.at(lNum.size() - i);
            }
            else
            {
                if(trackZeroes(lNumPart, 0) == lNumPart.size())
                {
                    break;
                }

                digit = 0;
                ++narrower;
            }

            lNumPart.emplace_back(digit);
            continue;
        }

        size_t res = 0;
        size_t zeroTrack = 0;

        while(!isVectorLess(lNumPart, rNumPart)) // TODO: Post?
        {
            size_t shift = 0;

            if(lNumPart.size() > rNumPart.size())
            {
                shift = lNumPart.size() - rNumPart.size();
            }

            // TODO: Fix shift types
            quotHelperSubtract(lNumPart, rNumPart, shift);
            ++res;

            zeroTrack = trackZeroes(lNumPart, zeroTrack); // TODO: Rename

            if(zeroTrack == lNumPart.size())
            {
                // lNumPart is full of zeroes
                lNumPart.clear();
                break;
            }
        }

        ++i;
        quotNum.emplace_back(res);

        if(i <= lNum.size())
        {
            lNumPart.emplace_back(lNum.at(lNum.size() - i));
            continue;
        }

        if(lNumPart.empty() || narrower >= precision + _precision + 1)
        {
            break;
        }

        lNumPart.emplace_back(0);
        ++narrower;
    }

    reverse(quotNum.begin(), quotNum.end());

    return quotNum;
}

bool BigNumber::isVectorGreater(const vector<uint8_t>& lNum,
                                size_t lFractPos,
                                const vector<uint8_t>& rNum,
                                size_t rFractPos)
{
    // FIXME: Copypaste
    size_t lShift = 0;
    size_t rShift = 0;
    size_t i = 0;

    if(lNum.size() > rNum.size())
    {
        lShift = lNum.size() - rNum.size();
        i = rNum.size();
    }
    else
    {
        rShift = rNum.size() - lNum.size();
        i = lNum.size();
    }

    while(i > 0)
    {
        --i;

        if(lNum.at(i + lShift) > rNum.at(i + rShift))
        {
            return true;
        }

        if(lNum.at(i + lShift) < rNum.at(i + rShift))
        {
            return false;
        }
    }

    return lFractPos > rFractPos;
}

bool BigNumber::isVectorLess(const vector<uint8_t>& lNum,
                             size_t lFractPos,
                             const vector<uint8_t>& rNum,
                             size_t rFractPos)
{
    size_t lShift = 0;
    size_t rShift = 0;
    size_t i = 0;

    if(lNum.size() > rNum.size())
    {
        lShift = lNum.size() - rNum.size();
        i = rNum.size();
    }
    else
    {
        rShift = rNum.size() - lNum.size();
        i = lNum.size();
    }

    while(i > 0)
    {
        --i;

        if(lNum.at(i + lShift) < rNum.at(i + rShift))
        {
            return true;
        }

        if(lNum.at(i + lShift) > rNum.at(i + rShift))
        {
            return false;
        }
    }

    return lFractPos < rFractPos;
}

bool BigNumber::isVectorLess(const vector<uint8_t>& lNum,
                             const vector<uint8_t>& rNum)
{
    size_t lShift = 0;
    size_t rShift = 0;

    if(lNum.size() < rNum.size())
    {
        return true;
    }

    uint8_t lval = 0;
    uint8_t rval = 0;

    if(lNum.size() > rNum.size())
    {
        rShift = lNum.size() - rNum.size();
    }
    else
    {
        lShift = rNum.size() - lNum.size();
    }

    for(size_t i = 0; i < lNum.size(); ++i)
    {
        if(i >= lShift && i < lNum.size() + lShift)
        {
            lval = lNum.at(i - lShift);
        }

        if(i >= rShift && i < rNum.size() + rShift)
        {
            rval = rNum.at(i - rShift);
        }

        if(lval < rval)
        {
            return true;
        }

        if(lval > rval)
        {
            return false;
        }
    }

    return false;
}
