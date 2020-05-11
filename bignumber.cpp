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

    vector<uint8_t> sum = sumOfVectors(_numIntPart,
                                       other._numIntPart, extender,
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

    return num;
}

BigNumber BigNumber::operator *(const BigNumber& other) const
{
    // FIXME: After * 0 pop vector
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

    num._numIntPart = quotOfVectors(_numIntPart, other._numIntPart, lUp, rUp, narrower);
    num._fractPos = narrower; //abs(int64_t(_fractPos - other._fractPos)); // TODO: Rename

    return num;
}

bool BigNumber::operator >(const BigNumber& other) const
{
    if(_sign == Sign::positive && other._sign == Sign::negative)
    {
        return true;
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

// TODO: RM?
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

//        if(sumNum.empty() && sum == 0)
//        {
//            ++drift;
//        }
//        else
        {
            sumNum.emplace_back(sum);
        }
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
    // FIXME: Zero case
    vector<vector<uint8_t>> prodBuf;
    vector<uint8_t> prodNum;
    uint32_t shift = 0; // TODO: Rename

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

    // Insert in reversed order
    lNumPart.insert(lNumPart.begin(), lNum.rbegin(), lNum.rend() - lShift);
    rNumPart.insert(rNumPart.begin(), rNum.rbegin(), rNum.rend());

    size_t i = lNumPart.size();

    while(true) // FIXME: ouf
    {
        if(isVectorLesser(lNumPart, rNumPart))
        {
            uint8_t digit;

            quotNum.emplace_back(0);

            if(i < lNum.size())
            {
                lNum.at(lNum.size() - i);
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
            ++i; // FIXME: Dupe
            continue;
        }

        size_t res = 0;
        size_t zeroTrack = 0;

        while(!isVectorLesser(lNumPart, rNumPart)) // TODO: Post?
        {
            lNumPart = diffOfVectors(lNumPart, rNumPart, 0, 0);
            ++res;

            zeroTrack = trackZeroes(lNumPart, zeroTrack); // TODO: Rename

            if(zeroTrack == lNumPart.size())
            {
                // lNumPart is full of zeroes
                lNumPart.clear();
                break;
            }
        }

        quotNum.emplace_back(res);
        ++i;

        if(i <= lNum.size())
        {
            lNumPart.emplace_back(lNum.at(lNum.size() - i));
        }
        else
        if(lNumPart.empty() ||
           quotNum.size() > 50) // TODO: Add precision const
        {
            break;
        }
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
