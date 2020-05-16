#pragma once

#include "bignumbertypes.h"

#include <cstdint>
#include <functional>
#include <iostream>
#include <vector>

class BigNumber final
{
public:
    explicit BigNumber();
    explicit BigNumber(std::vector<uint8_t> numIntPart, size_t fractPos,
                       bigNumber::Sign sign, bigNumber::Status status);

    // Check if number == 0
    bool isZero() const;

    // +/- sign
    bigNumber::Sign getSign() const;
    // Inf, NaN, Normal statuses
    bigNumber::Status getStatus() const;

    // Calculate square root with precision based on number length
    BigNumber sqrt() const;
    // Round a number to (_precision) decimal places
    BigNumber round() const;

    BigNumber operator -() const;

    BigNumber operator +(const BigNumber& other) const;
    BigNumber operator -(const BigNumber& other) const;
    BigNumber operator *(const BigNumber& other) const;
    BigNumber operator /(const BigNumber& other) const;

    bool operator >(const BigNumber& other) const;
    bool operator <(const BigNumber& other) const;
    bool operator ==(const BigNumber& other) const;
    bool operator !=(const BigNumber& other) const;

    // Output number as floating point decimal
    friend std::ostream& operator <<(std::ostream& os, const BigNumber& num);

private:
    // Sum two BigNumbers irrespective of sign or status
    static BigNumber sum(const BigNumber& leftNum, const BigNumber& rightNum);
    // Subtract two BigNumbers
    static BigNumber diff(const BigNumber& leftNum, const BigNumber& rightNum);

    // Determine sign of a product or quotient
    static bigNumber::Sign prodQuotSign(const bigNumber::Sign& lSign,
                                        const bigNumber::Sign& rSign);

    // Removes nonsignificant digit
    static void popZeroes(std::vector<uint8_t>& vec);

    // Calc sum of vectors. Additional digit will be written to (carry)
    static std::vector<uint8_t> sumOfVectors(const std::vector<uint8_t>& lNum,
                                             const std::vector<uint8_t>& rNum,
                                             uint8_t& carry,
                                             size_t lShift,
                                             size_t rShift);
    // Calcs difference of vectors. Additional digit will be written to (carry)
    static std::vector<uint8_t> diffOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              uint8_t& carry,
                                              size_t lShift,
                                              size_t rShift);
    // Calcs product of vectors. Additional digit will be written to (carry)
    static std::vector<uint8_t> prodOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              uint8_t& carry);

    static std::vector<uint8_t> prodHelperMultiply(const std::vector<uint8_t>& lNum,
                                                   uint8_t multiplier);

    // Calcs quotient of vectors
    // Decimal point position will be written to (carry)
    static std::vector<uint8_t> quotOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              size_t lUp,
                                              size_t rUp, // TODO: Fix naming
                                              size_t precision,
                                              uint8_t& narrower);

    static bool quotHelperLess(const std::vector<uint8_t>& lNum,
                               const std::vector<uint8_t>& rNum);

    static void quotHelperSubtract(std::vector<uint8_t>& lNum,
                                   const std::vector<uint8_t>& rNum,
                                   size_t rShift);

    static bool comparOfVectors(const std::vector<uint8_t>& lNum,
                                size_t lFractPos,
                                const std::vector<uint8_t>& rNum,
                                size_t rFractPos,
                                const std::function<bool(int, int)>& functor);

    static size_t trackZeroes(const std::vector<uint8_t>& vec, size_t pos);

private:
    // Number of digits after decimal separator for round() and arithmetics
    static constexpr size_t _precision = 30;
    std::vector<uint8_t> _numIntPart;
    size_t _fractPos;
    bigNumber::Sign _sign;
    bigNumber::Status _status;
};
