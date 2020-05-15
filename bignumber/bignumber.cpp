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
    _status(Status::normal),
    _decimalPointFlag(false)
{
}

BigNumber::BigNumber(vector<uint8_t> numIntPart,
                     size_t fractPos,
                     bool decimalPointFlag,
                     Sign sign,
                     Status status):
    _numIntPart(move(numIntPart)),
    _fractPos(fractPos),
    _sign(sign),
    _status(status),
    _decimalPointFlag(decimalPointFlag)
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
    if(_status != Status::normal)
    {
        return *this;
    }

    if(_sign == Sign::negative || isZero())
    {
        return BigNumber{_numIntPart, _fractPos, _decimalPointFlag, _sign, Status::nan};
    }

    static const BigNumber Big0_5{{5}, 1, false, Sign::positive, Status::normal};

    // Newton's method
    BigNumber dividend = *this;
    BigNumber val = *this;
    BigNumber last;

    // TODO: Precision have to depends on num length
    constexpr int prec = 64;
//    constexpr int prec = 10;
    int i = 0;

    do
    {
        ++i;
        last = val;
        val = (val + dividend / val) * Big0_5;
    }
    while(i < prec); // Precision

    return val;
}

BigNumber BigNumber::round() const
{
    if(_status != Status::normal)
    {
        return *this;
    }

    constexpr int precision = 30;

    if(_fractPos <= precision)
    {
        return *this;
    }

    size_t pos = _fractPos - precision;
    size_t digitPos = pos - 1;
    size_t digit;

    if(digitPos < _numIntPart.size())
    {
        digit = _numIntPart.at(digitPos);
    }
    else
    {
        return BigNumber{{}, 0, false, _sign, _status};
    }

    std::vector<uint8_t> num;
    num.insert(num.begin(), _numIntPart.begin() + pos, _numIntPart.end());
    size_t numFractPos = _fractPos - (_numIntPart.size() - num.size());
    BigNumber res{num, numFractPos, false, _sign, _status};

    if(digit < 5)
    {
        return res;
    }

    BigNumber rounder{{1}, numFractPos, false, _sign, _status};

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
    if(_status != Status::normal)
    {
        return *this;
    }

    if(other._status != Status::normal)
    {
        return other;
    }

    // Sign managment and select proper function
    // diff() has to be called with correct number signs
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
    if(_status != Status::normal)
    {
        return *this;
    }

    if(other._status != Status::normal)
    {
        return other;
    }

    // Sign managment and select proper function
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
    if(_status != Status::normal)
    {
        return *this;
    }

    if(other._status != Status::normal)
    {
        return other;
    }

    // FIXME: After * 0 pop vector
    BigNumber num;
    uint8_t extender = 0;

    if(_sign == other._sign)
    {
        num._sign = Sign::positive;
    }
    else
    {
        num._sign = Sign::negative;
    }

    num._fractPos = _fractPos + other._fractPos;

    vector<uint8_t> prod = prodOfVectors(_numIntPart, other._numIntPart, extender);

    size_t zeroPos = trackZeroes(prod, 0);
    size_t driftPos = min(zeroPos, num._fractPos);

    num._numIntPart.insert(num._numIntPart.begin(), prod.begin() + driftPos, prod.end());
    num._fractPos -= driftPos; // TODO: Check

    if(extender != 0)
    {
        num._numIntPart.emplace_back(extender);
    }

    return num;
}

BigNumber BigNumber::operator /(const BigNumber& other) const
{
    if(_status != Status::normal)
    {
        return *this;
    }

    if(other._status != Status::normal)
    {
        return other;
    }

    Sign sign = Sign::positive;

    if(_sign == other._sign)
    {
        sign = Sign::positive;
    }
    else
    {
        sign = Sign::negative;
    }

    if(other.isZero())
    {
        if(isZero())
        {
            return BigNumber{_numIntPart, _fractPos, _decimalPointFlag, sign, Status::nan};
        }

        return BigNumber{_numIntPart, _fractPos, _decimalPointFlag, sign, Status::inf};
    }

    // FIXME: Zero case
    BigNumber num;
    uint8_t narrower = 0;

    num._sign = sign;

    // TODO: Uncomment
//    if(*this == other)
//    {
//        num._numIntPart.emplace_back(1);
//        num._fractPos = 0;

//        return num;
//    }

    size_t lUp = 0;
    size_t rUp = 0;

    if(_fractPos > other._fractPos)
    {
        rUp = _fractPos - other._fractPos;
    }

    if(other._fractPos > _fractPos)
    {
        lUp = other._fractPos - _fractPos;
    }

    num._numIntPart = quotOfVectors(_numIntPart, other._numIntPart, 0, rUp, narrower);
    num._fractPos = narrower - lUp; //abs(int64_t(_fractPos - other._fractPos)); // TODO: Rename

    return num;
}

bool BigNumber::operator >(const BigNumber& other) const
{
    if(_sign == Sign::positive && other._sign == Sign::negative)
    {
        return true;
    }

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
    if(_sign == Sign::negative && other._sign == Sign::positive)
    {
        return true;
    }

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

    return isVectorLesser(_numIntPart, _fractPos,
                          other._numIntPart, other._fractPos);
}

bool BigNumber::operator ==(const BigNumber& other) const
{
    if(_sign != other._sign && !isZero())
    {
        return false;
    }

    if(_status == Status::inf)
    {
        return true;
    }

    if(_status == Status::nan)
    {
        return false;
    }

    // TODO: Check
    if(_fractPos != other._fractPos ||
       _numIntPart.size() != other._numIntPart.size())
    {
        return false;
    }

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
    // Output minus sign if this number is negative and has at least one digit
    // FIXME: -0.0
    // FIXME: Status::null
    if(num.isZero())
    {
        os << '0';

        return os;
    }

    if(num._sign == Sign::negative)
//       &&
//       (!num._numIntPart.empty() || num._numFractPart.empty()))
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

    size_t i;
    const auto& loc = use_facet<numpunct<char>>(cout.getloc());

    if(num._fractPos >= num._numIntPart.size())
    {
        os << '0';
        os << loc.decimal_point();
        i = num._fractPos;
    }
    else
    {
        i = num._numIntPart.size();
    }

    while(i > 0)
    {
        --i;

        if(i < num._numIntPart.size())
        {
            os << int(num._numIntPart.at(i));
        }
        else
        {
            os << '0';
        }

        if(i == num._fractPos && i != 0)
        {
            os << loc.decimal_point();
        }
    }

    return os;
}

BigNumber BigNumber::sum(const BigNumber& leftNum, const BigNumber& rightNum)
{
    BigNumber num;
    uint8_t extender = 0;
    uint32_t lShift = 0;
    uint32_t rShift = 0;

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

    vector<uint8_t> sum = sumOfVectors(leftNum._numIntPart,
                                       rightNum._numIntPart, extender,
                                       lShift, rShift);

    size_t zeroPos = trackZeroes(sum, 0);
    size_t driftPos = min(zeroPos, num._fractPos);

    num._numIntPart.insert(num._numIntPart.begin(), sum.begin() + driftPos, sum.end());
    num._fractPos -= driftPos; // TODO: Check

    if(extender != 0)
    {
        num._numIntPart.emplace_back(extender);
    }

    return num;
}

BigNumber BigNumber::diff(const BigNumber& leftNum, const BigNumber& rightNum)
{
    BigNumber num;
    uint8_t narrower = 0;
    uint32_t lShift = 0;
    uint32_t rShift = 0;

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

void BigNumber::setSign(Sign sign)
{
    _sign = sign;
}

// NOTE: Dupe from builder
void BigNumber::popZeroes(vector<uint8_t>& vec)
{
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
                                        uint8_t& extender,
                                        uint32_t lShift, // TODO: size_t?
                                        uint32_t rShift)
{
    size_t maxSize = max(lNum.size() + lShift, rNum.size() + rShift);
    vector<uint8_t> sumNum;
    sumNum.reserve(maxSize);
    size_t drift = 0;

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

        sum = lval + rval + extender;

        if(sum > 9)
        {
            extender = 1;
            sum -= 10;
        }
        else
        {
            extender = 0;
        }

        sumNum.emplace_back(sum);
    }

    return sumNum;
}

vector<uint8_t> BigNumber::diffOfVectors(const vector<uint8_t>& lNum,
                                         const vector<uint8_t>& rNum,
                                         uint8_t& narrower, // TODO: RM
                                         uint32_t lShift,
                                         uint32_t rShift)
{
    size_t maxSize = max(lNum.size() + lShift, rNum.size() + rShift);
    vector<uint8_t> diffNum(maxSize);

    for(size_t i = 0; i < maxSize; ++i)
    {
        int8_t diff = 0;
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

        diff = lval - rval - narrower;

        if(diff < 0)
        {
            narrower = 1;

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
            narrower = 0;
        }

        diffNum.at(i) = diff;
    }

    popZeroes(diffNum);

    return diffNum;
}

vector<uint8_t> BigNumber::diffOfVectors(const vector<uint8_t>& lNum,
                                              const vector<uint8_t>& rNum,
                                              uint32_t lShift,
                                              uint32_t rShift)
{
    size_t maxSize = max(lNum.size() + lShift, rNum.size() + rShift);
    vector<uint8_t> diffNum(maxSize);
    uint8_t narrower = 0;

    size_t i = maxSize;

    while(i > 0)
    {
        int8_t diff = 0;
        uint8_t lval = 0;
        uint8_t rval = 0;

        --i;

        if(i >= lShift && i < lNum.size() + lShift)
        {
            lval = lNum.at(i - lShift);
        }

        if(i >= rShift && i < rNum.size() + rShift)
        {
            rval = rNum.at(i - rShift);
        }

        diff = lval - rval - narrower;

        if(diff < 0)
        {
            narrower = 1;

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
            narrower = 0;
        }

        diffNum.at(i) = diff;
    }

//    popZeroes(diffNum);

    return diffNum;
}

vector<uint8_t> BigNumber::prodHelper(const vector<uint8_t>& lNum,
                                      uint8_t multiplier)
{
    uint8_t extender = 0;
    vector<uint8_t> prodVec;

    prodVec.reserve(lNum.size() + 1);

    for(const auto& next : lNum)
    {
        uint8_t prod = (next * multiplier) + extender;

        if(prod > 9)
        {
            extender = prod / 10;
            prod -= extender * 10;
        }
        else
        {
            extender = 0;
        }

        prodVec.emplace_back(prod);
    }

    if(extender != 0)
    {
        prodVec.emplace_back(extender);
    }

    return prodVec;
}

vector<uint8_t> BigNumber::prodOfVectors(const vector<uint8_t>& lNum,
                                         const vector<uint8_t>& rNum,
                                         uint8_t& extender)
{
    vector<uint8_t> prodNum;
    uint32_t shift = 0; // TODO: Rename

    // TODO: Reserve
//    prodNum.reserve(lNum.size() + rNum.size() - 1);

    for(const auto& next : rNum)
    {
        prodNum = sumOfVectors(prodNum, prodHelper(lNum, next),
                               extender, 0, shift);

        if(extender != 0)
        {
            prodNum.emplace_back(extender);
            extender = 0;
        }

        ++shift;
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
                                         uint8_t& narrower) // TODO: RM
{
    // FIXME: Empty case
    vector<vector<uint8_t>> quotBuf;
    vector<uint8_t> quotNum;
    size_t lShift = 0;
    size_t rShift = 0;

    // FIXME: reserve
//    quotBuf.reserve()

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
        if(isVectorLesser(lNumPart, rNumPart))
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

        while(!isVectorLesser(lNumPart, rNumPart)) // TODO: Post?
        {
            size_t shift = 0;

            if(lNumPart.size() > rNumPart.size())
            {
                shift = lNumPart.size() - rNumPart.size();
            }

            // TODO: Fix shift types
            lNumPart = diffOfVectors(lNumPart, rNumPart, 0, shift);
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

        if(lNumPart.empty() ||
           quotNum.size() > 40) // TODO: Add precision const
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

bool BigNumber::isVectorLesser(const vector<uint8_t>& lNum,
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

bool BigNumber::isVectorLesser(const vector<uint8_t>& lNum,
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


//    if(lNum.size() > rNum.size())
//    {
//        lShift = lNum.size() - rNum.size();
//        i = rNum.size();
//    }
//    else
//    {
//        rShift = rNum.size() - lNum.size();
//        i = lNum.size();
//    }

//    for(size_t i = 0; i < lNum.size(); ++i)
//    {
//        if(lNum.at(i + lShift) < rNum.at(i + rShift))
//        {
//            return true; // BUG
//        }

//        if(lNum.at(i + lShift) > rNum.at(i + rShift))
//        {
//            return false;
//        }
//    }

    return false;
}

//// NOTE: Reversal
//// TODO: Must be equal by size, rename
//bool BigNumber::isVectorLesser(const vector<uint8_t>& lNum,
//                               const vector<uint8_t>& rNum)
//{
//    if(lNum.size() < rNum.size())
//    {
//        return true;
//    }

//    if(lNum.size() > rNum.size())
//    {
//        return false;
//    }

//    for(size_t i = 0; i < lNum.size(); ++i)
//    {
//        if(lNum.at(i) < rNum.at(i))
//        {
//            return true;
//        }

//        if(lNum.at(i) > rNum.at(i))
//        {
//            return false;
//        }
//    }

//    // Equal
//    return false;
//}

//bool BigNumber::isVectorLesser(const vector<uint8_t>& lNum,
//                               const vector<uint8_t>& rNum)
//{
//    // NOTE: Shift is unnecessary
//    size_t lShift = 0;
//    size_t rShift = 0;
//    size_t i = 0;

//    if(lNum.size() > rNum.size())
//    {
//        lShift = lNum.size() - rNum.size();
//        i = rNum.size();
//    }
//    else
//    {
//        rShift = rNum.size() - lNum.size();
//        i = lNum.size();
//    }

//    while(i > 0)
//    {
//        --i;

//        if(lNum.at(i + lShift) < rNum.at(i + rShift))
//        {
//            return true;
//        }

//        if(lNum.at(i + lShift) > rNum.at(i + rShift))
//        {
//            return false;
//        }
//    }

//    return false;
//}
