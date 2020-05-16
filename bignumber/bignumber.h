#pragma once

#include "bignumbertypes.h"
#include "numvector.h"

#include <iostream>

class BigNumber final
{
public:
    explicit BigNumber();
    explicit BigNumber(numVector::NumVector numIntPart, size_t fractPos,
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

private:
    // Number of digits after decimal separator for round() and arithmetics
    static constexpr size_t _precision = 30;
    numVector::NumVector _numIntPart;
    size_t _fractPos;
    bigNumber::Sign _sign;
    bigNumber::Status _status;
};
