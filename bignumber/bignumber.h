#pragma once

#include "bignumbertypes.h"
#include "numvector.h"

#include <iostream>
#include <string>

class BigNumber final
{
    // Any non-zero normal numbers have to be constructed with BigNumberBuilder
    friend class BigNumberBuilder;
    explicit BigNumber(numVector::NumVector numIntPart, size_t fractPos,
                       bigNumber::Sign sign, bigNumber::Status status);

public:
    explicit BigNumber(); // Positive zero
    explicit BigNumber(bigNumber::Sign sign, bigNumber::Status status);
    // Ctor pass (num) to BigNumberBuilder to create number
    explicit BigNumber(const std::string& num);

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
    static BigNumber sum(const BigNumber& lNum, const BigNumber& rNum);
    // Subtract two BigNumbers irrespective of status ONLY
    static BigNumber diff(const BigNumber& lNum, const BigNumber& rNum);
    // Multiply two BigNumbers irrespective of sign or status
    static BigNumber multiply(const BigNumber& lNum, const BigNumber& rNum);
    // Divide two BigNumbers irrespective of sign or status
    static BigNumber divide(const BigNumber& lNum, const BigNumber& rNum);

    // Determine sign of a product or quotient
    static bigNumber::Sign prodQuotSign(const bigNumber::Sign& lSign,
                                        const bigNumber::Sign& rSign);

private:
    // Number of digits after decimal separator for round() and arithmetics
    static constexpr size_t _precision = 30;
    numVector::NumVector _number; // Number without insignificat zeroes
    size_t _fractPos; // Decimal point position (0 for integer)
    bigNumber::Sign _sign; // +/-
    bigNumber::Status _status; // Inf, NaN, Normal
};
