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
    _decimalPointFlag(false)
{
}

BigNumber::BigNumber(vector<uint8_t> numIntPart,
                     size_t fractPos,
                     bool decimalPointFlag,
                     Sign sign):
    _numIntPart(move(numIntPart)),
    _fractPos(fractPos),
    _sign(sign),
    _decimalPointFlag(decimalPointFlag)
{
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
    BigNumber num;
    uint8_t extender = 0;
    uint32_t lShift = 0;
    uint32_t rShift = 0;

    if(_fractPos > other._fractPos)
    {
        rShift = _fractPos - other._fractPos;
        num._fractPos = _fractPos;
    }
    else
    {
        lShift = other._fractPos - _fractPos;
        num._fractPos = other._fractPos;
    }

    num._numIntPart = sumOfVectors(_numIntPart,
                                   other._numIntPart, extender,
                                   lShift, rShift);

    if(extender != 0)
    {
        num._numIntPart.emplace_back(extender);
    }

    return num;
}

BigNumber BigNumber::operator -(const BigNumber& other) const
{
    BigNumber num;
    uint8_t narrower = 0;
    uint32_t lShift = 0;
    uint32_t rShift = 0;

    // TODO: Copypaste from op +

    if(_fractPos > other._fractPos)
    {
        rShift = _fractPos - other._fractPos;
        num._fractPos = _fractPos;
    }
    else
    {
        lShift = other._fractPos - _fractPos;
        num._fractPos = other._fractPos;
    }

    if(*this < other)
    {
        num._numIntPart = diffOfVectors(other._numIntPart,
                                        _numIntPart, narrower,
                                        rShift, lShift);
        num._sign = Sign::negative;
    }
    else
    {
        num._numIntPart = diffOfVectors(_numIntPart,
                                        other._numIntPart, narrower,
                                        lShift, rShift);
    }


//    if(narrower != 0)
//    {
//    }

    return num;
}

BigNumber BigNumber::operator *(const BigNumber& other) const
{
    // FIXME: Atfter * 0 pop vector
    BigNumber num;
    uint8_t extender = 0;

    if(num._sign == other._sign)
    {
        num._sign = Sign::positive;
    }
    else
    {
        num._sign = Sign::negative;
    }

    num._fractPos = _fractPos + other._fractPos;

    // FIXME: Add trim()
    num._numIntPart = prodOfVectors(_numIntPart, other._numIntPart, extender);

    if(extender != 0)
    {
        num._numIntPart.emplace_back(extender);
    }

    return num;
}

BigNumber BigNumber::operator /(const BigNumber& other) const
{
    // FIXME: Zero case
    BigNumber num;
    uint8_t narrower = 0;

    if(num._sign == other._sign)
    {
        num._sign = Sign::positive;
    }
    else
    {
        num._sign = Sign::negative;
    }

    if(*this == other)
    {
        num._numIntPart.emplace_back(1);

        return num;
    }

//    vector<vector<uint8_t>> quotBuf;
    vector<uint8_t> quotNum;
    size_t rightShift = 0;

    // FIXME: reserve
//    quotBuf.reserve()

//    if(lNum < rNum)
//    {
//        if(rNum.size() == lNum.size())
//        {
//            rightShift = 1;
//        }
//        else
//        {
//            rightShift = rNum.size() - lNum.size();
//        }
//    }

//    // FIXME: Check everywere and mb not here
//    if(!_numFractPart.empty() || !other._numFractPart.empty())
//    {
//        num._numFractPart = quotOfVectors(_numFractPart,
//                                          other._numFractPart, narrower);
//    }

//    num._numIntPart = quotOfVectors(_numIntPart,
//                                    other._numIntPart, narrower);

    return num;
}

bool BigNumber::operator >(const BigNumber& other) const
{
    if(_sign == Sign::positive && other._sign == Sign::negative)
    {
        return true;
    }

    if(_numIntPart.size() - _fractPos < other._numIntPart.size() - other._fractPos)
    {
        return false;
    }

    if(_numIntPart.size() - _fractPos > other._numIntPart.size() - other._fractPos)
    {
        return true;
    }

    // FIXME: Copypaste
    uint32_t lShift = 0;
    uint32_t rShift = 0;
    size_t i = 0;

    if(_numIntPart.size() > other._numIntPart.size())
    {
        lShift = _numIntPart.size() - other._numIntPart.size();
        i = other._numIntPart.size();
    }
    else
    {
        rShift = other._numIntPart.size() - _numIntPart.size();
        i = _numIntPart.size();
    }

    while(i > 0)
    {
        --i;

        if(_numIntPart.at(i + lShift) > other._numIntPart.at(i + rShift))
        {
            return true;
        }

        if(_numIntPart.at(i + lShift) < other._numIntPart.at(i + rShift))
        {
            return false;
        }
    }

    return _fractPos > other._fractPos;
}

bool BigNumber::operator <(const BigNumber& other) const
{
    if(_sign == Sign::negative && other._sign == Sign::positive)
    {
        return true;
    }

    if(_numIntPart.size() - _fractPos > other._numIntPart.size() - other._fractPos)
    {
        return false;
    }

    if(_numIntPart.size() - _fractPos < other._numIntPart.size() - other._fractPos)
    {
        return true;
    }

    uint32_t lShift = 0;
    uint32_t rShift = 0;
    size_t i = 0;

    if(_numIntPart.size() > other._numIntPart.size())
    {
        lShift = _numIntPart.size() - other._numIntPart.size();
        i = other._numIntPart.size();
    }
    else
    {
        rShift = other._numIntPart.size() - _numIntPart.size();
        i = _numIntPart.size();
    }

    while(i > 0)
    {
        --i;

        if(_numIntPart.at(i + lShift) < other._numIntPart.at(i + rShift))
        {
            return true;
        }

        if(_numIntPart.at(i + lShift) > other._numIntPart.at(i + rShift))
        {
            return false;
        }
    }

    return _fractPos < other._fractPos;
}

bool BigNumber::operator ==(const BigNumber& other) const
{
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

ostream& operator <<(ostream& os, const BigNumber& num)
{
    // Output minus sign if this number is negative and has at least one digit
    // FIXME: -0.0
    // FIXME: Status::null
    if(num._numIntPart.empty())
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

    size_t i = num._numIntPart.size();

    while(i > 0)
    {
        --i;

        os << int(num._numIntPart.at(i));

        if(i == num._fractPos && i != 0)
        {
            const auto& loc = use_facet<numpunct<char>>(cout.getloc());

            os << loc.decimal_point();
        }
    }

    return os;
}

void BigNumber::setSign(Sign sign)
{
    _sign = sign;
}

vector<uint8_t> BigNumber::sumOfVectors(const vector<uint8_t>& lNum,
                                        const vector<uint8_t>& rNum,
                                        uint8_t& extender,
                                        uint32_t lShift,
                                        uint32_t rShift)
{
    size_t maxSize = max(lNum.size() + lShift, rNum.size() + rShift);
    vector<uint8_t> sumNum(maxSize);

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

        sumNum.at(i) = sum;
    }

    return sumNum;
}

vector<uint8_t> BigNumber::diffOfVectors(const vector<uint8_t>& lNum,
                                         const vector<uint8_t>& rNum,
                                         uint8_t& narrower,
                                         uint32_t lShift,
                                         uint32_t rShift)
{
    size_t maxSize = max(lNum.size() + lShift, rNum.size() + rShift);
    vector<uint8_t> diffNum(maxSize); // TODO: Double check

//    // FIXME: Shitcode
//    const vector<uint8_t>& topNum = lNum.size() < rNum.size() ? rNum : lNum;
//    const vector<uint8_t>& botNum = lNum.size() < rNum.size() ? lNum : rNum;

//    if(lNum.size() < rNum.size())
//    {
//        swap(lShift, rShift);
//    }

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
    // FIXME: Zero case
    vector<vector<uint8_t>> prodBuf;
    vector<uint8_t> prodNum;
    uint32_t shift = 0;

    prodBuf.reserve(rNum.size());
//    prodNum.reserve(lNum.size() + rNum.size() - 1);

    for(const auto& next : rNum)
    {
        prodBuf.emplace_back(prodHelper(lNum, next));
    }

    // NOTE: Performance?
    for(const auto& next : prodBuf)
    {
        prodNum = sumOfVectors(prodNum, next, extender, 0, shift);

        if(extender != 0)
        {
            prodNum.emplace_back(extender);
            extender = 0;
        }

        ++shift;
    }

    return prodNum;
}

vector<uint8_t> BigNumber::quotOfVectors(const vector<uint8_t>& lNum,
                                         const vector<uint8_t>& rNum,
                                         uint8_t& narrower)
{
    // FIXME: Zero case
    vector<vector<uint8_t>> quotBuf;
    vector<uint8_t> quotNum;
    size_t rightShift = 0;

    // FIXME: reserve
//    quotBuf.reserve()

    if(lNum < rNum)
    {
        if(rNum.size() == lNum.size())
        {
            rightShift = 1;
        }
        else
        {
            rightShift = rNum.size() - lNum.size();
        }
    }

//    do
//    {

//    }
//    while()


//    int norm = base / (b1.a.back() + 1);
//    bigint a = a1.abs() * norm;
//    bigint b = b1.abs() * norm;
//    bigint q, r;
//    q.a.resize(a.a.size());

//    for (int i = a.a.size() - 1; i >= 0; i--) {
//        r *= base;
//        r += a.a[i];
//        int s1 = r.a.size() <= b.a.size() ? 0 : r.a[b.a.size()];
//        int s2 = r.a.size() <= b.a.size() - 1 ? 0 : r.a[b.a.size() - 1];
//        int d = ((long long) base * s1 + s2) / b.a.back();
//        r -= b * d;
//        while (r < 0)
//            r += b, --d;
//        q.a[i] = d;
//    }

//    q.sign = a1.sign * b1.sign;
//    r.sign = a1.sign;
//    q.trim();
//    r.trim();
//    return make_pair(q, r / norm);

    return quotNum;
}
