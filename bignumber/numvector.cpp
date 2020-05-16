#include "numvector.h"

#include <algorithm>

using namespace numVector;
using namespace std;

NumVector numVector::sumOfVectors(const NumVector& lNum,
                                        const NumVector& rNum,
                                        uint8_t& carry,
                                        size_t lShift,
                                        size_t rShift)
{
    // Sum vectors. Additional digit will be written to (carry)
    NumVector sumNum;
    size_t maxSize = max(lNum.size() + lShift, rNum.size() + rShift);

    // Reserve maximum possible capacity
    sumNum.reserve(maxSize + 1);

    // Sum vectors digit by digit
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

NumVector numVector::diffOfVectors(const NumVector& lNum,
                                         const NumVector& rNum,
                                         uint8_t& carry,
                                         size_t lShift,
                                         size_t rShift)
{
    // Subtract vectors. Additional digit will be written to (carry)
    NumVector diffNum; // Result dummy
    size_t maxSize = max(lNum.size() + lShift, rNum.size() + rShift);

    // Reserve maximum possible capacity
    diffNum.reserve(maxSize);

    // Subtract vectors digit by digit
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

NumVector numVector::prodOfVectors(const NumVector& lNum,
                                         const NumVector& rNum,
                                         uint8_t& carry)
{
    NumVector prodNum; // Result dummy
    size_t rShift = 0; // Shift of right number in sum to shape correct column

    // Reserve maximum possible capacity
    prodNum.reserve(lNum.size() + rNum.size());

    // Multiply vectors with column-like multiplication
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

NumVector numVector::prodHelperMultiply(const NumVector& lNum,
                                              uint8_t multiplier)
{
    // Multiply vector by (multiplier)
    NumVector prodVec; // Result dummy
    uint8_t carry = 0;

    // Reserve maximum possible capacity
    prodVec.reserve(lNum.size() + 1);

    // Multiply digit by digit
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

NumVector numVector::quotOfVectors(const NumVector& lNum,
                                         const NumVector& rNum,
                                         size_t lShift,
                                         size_t rShift,
                                         size_t precision,
                                         size_t& decPos)
{
    NumVector quotNum; // Result dummy
    size_t lAddShift = 0;

    // Reserve approximate possible capacity (can be significantly different)
    quotNum.reserve(lNum.size());

    // Determine additional shift to shape correct column
    if(rNum.size() < lNum.size())
    {
        lAddShift = lNum.size() - rNum.size();
    }

    // Parts of numbers
    NumVector dividend;
    NumVector divisor(rShift);

    lAddShift = max(int(int64_t(0)), int(lAddShift - rShift));

    // Insert in reversed order
    dividend.insert(dividend.begin(), lNum.rbegin(), lNum.rend() - lAddShift);
    divisor.insert(divisor.begin(), rNum.rbegin(), rNum.rend());

    // Divident's size have to be >= divisor's size
    if(dividend.size() < divisor.size())
    {
        decPos += divisor.size() - dividend.size();
        dividend.resize(divisor.size());
    }

    size_t i = dividend.size();

    // Divide vectors with column-like division
    while(true) // FIXME: ouf
    {
        if(quotHelperLess(dividend, divisor))
        {
            // Have to get additional digit to dividend
            uint8_t digit;

            ++i;

            // Add zero to the answer (ignore insignificant zeroes)
            if(!quotNum.empty())
            {
                quotNum.emplace_back(0);
            }

            // Get additional digit from lNum if possible or zero
            if(i <= lNum.size())
            {
                digit = lNum.at(lNum.size() - i);
            }
            else
            {
                if(trackZeroes(dividend, 0) == dividend.size())
                {
                    // Division completed without remainder
                    break;
                }

                digit = 0;
                ++decPos;
            }

            // Add additional digit to dividend
            dividend.emplace_back(digit);
            continue;
        }

        size_t res = 0;
        size_t zeroTrack = 0;

        do
        {
            size_t shift = 0;

            // Determine shift to shape correct column
            if(dividend.size() > divisor.size())
            {
                shift = dividend.size() - divisor.size();
            }

            // Subtract divisor from divident inplace
            quotHelperSubtract(dividend, divisor, shift);
            ++res;

            zeroTrack = trackZeroes(dividend, zeroTrack);

            if(zeroTrack == dividend.size())
            {
                // Division completed without remainder
                dividend.clear();
                break;
            }
        }
        while(!quotHelperLess(dividend, divisor));

        ++i;
        quotNum.emplace_back(res);

        // Add additional digit to dividend if possible
        if(i <= lNum.size())
        {
            dividend.emplace_back(lNum.at(lNum.size() - i));
            continue;
        }

        // Break if division completed w/o remainder or precision limit reached
        if(dividend.empty() || decPos >= lShift + precision + 1)
        {
            break;
        }

        // Heading to decimal part
        dividend.emplace_back(0);
        ++decPos;
    }

    // Restore correct order
    reverse(quotNum.begin(), quotNum.end());

    return quotNum;
}

bool numVector::quotHelperLess(const NumVector& lNum,
                               const NumVector& rNum)
{
    // Check if left vector number less than the right one
    // Vactors have to be prepared in quotOfVectors() function
    size_t rShift = 0;

    // If left vector are smaller than right then left vector number is less
    // It is valid because insignificant digits not presented in RIGHT vector
    if(lNum.size() < rNum.size())
    {
        return true;
    }

    uint8_t rval = 0;

    if(lNum.size() > rNum.size())
    {
        rShift = lNum.size() - rNum.size();
    }

    // Comare vectors digit to digit from the most significant one
    for(size_t i = 0; i < lNum.size(); ++i)
    {
        if(i >= rShift && i < rNum.size() + rShift)
        {
            rval = rNum.at(i - rShift);
        }

        if(lNum.at(i) < rval)
        {
            return true;
        }

        if(lNum.at(i) > rval)
        {
            return false;
        }
    }

    // Vectors are equal
    return false;
}

void numVector::quotHelperSubtract(NumVector& lNum,
                                   const NumVector& rNum,
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

bool numVector::compareOfVectors(const NumVector& lNum,
                                size_t lFractPos,
                                const NumVector& rNum,
                                size_t rFractPos,
                                const function<bool(int, int)>& functor)
{
    // If sizes of vector's integer part are different then numbers too
    // It is valid because insignificant digits not presented in vector
    if(functor(int(lNum.size() - lFractPos), int(rNum.size() - rFractPos)))
    {
        return true;
    }

    if(int(lNum.size() - lFractPos) != int(rNum.size() - rFractPos))
    {
        return false;
    }

    // Comare vectors digit to digit from the most significant one
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

        if(functor(lNum.at(i + lShift), rNum.at(i + rShift)))
        {
            return true;
        }

        if(lNum.at(i + lShift) != rNum.at(i + rShift))
        {
            return false;
        }
    }

    // If any digits in vector are same,
    // compare by number of digits after decimal point
    return functor(lFractPos, rFractPos);
}

void numVector::popZeroes(NumVector& vec)
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

size_t numVector::trackZeroes(const NumVector& vec, size_t pos)
{
    // Find position of the first non-zero digit from (pos) to the end of (vec)
    for(size_t i = pos; i < vec.size(); ++i)
    {
        if(vec.at(i) != 0)
        {
            return i;
        }
    }

    return vec.size();
}
