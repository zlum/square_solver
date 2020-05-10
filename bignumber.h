#pragma once

#include "bignumbertypes.h"

#include <cstdint>
#include <iostream>
#include <vector>

class BigNumber final
{
public:
    // TODO: Add reservre()
    explicit BigNumber();
    explicit BigNumber(
            std::vector<uint8_t> numIntPart,
            size_t fractPos,
            bool decimalPointFlag,
            bigNumber::Sign sign);

    // Changes sign of number or appends digit or decimal point
//    bool append(char symbol);

    BigNumber operator -() const;

    BigNumber operator +(const BigNumber& other) const;
    BigNumber operator -(const BigNumber& other) const;
    BigNumber operator *(const BigNumber& other) const;
    BigNumber operator /(const BigNumber& other) const;

    bool operator >(const BigNumber& other) const;
    bool operator <(const BigNumber& other) const;
    bool operator ==(const BigNumber& other) const;

    friend std::ostream& operator <<(std::ostream& os, const BigNumber& num);

private:
    void setSign(bigNumber::Sign sign); // TODO: RM?

    // Calcs sum of vectors
    static std::vector<uint8_t> sumOfVectors(const std::vector<uint8_t>& lNum,
                                             const std::vector<uint8_t>& rNum,
                                             uint8_t& extender,
                                             uint32_t lShift,
                                             uint32_t rShift);
    // Calcs difference of vectors
    static std::vector<uint8_t> diffOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              uint8_t& narrower,
                                              uint32_t lShift,
                                              uint32_t rShift);
    // Calcs product of vectors
    static std::vector<uint8_t> prodHelper(const std::vector<uint8_t>& lNum,
                                           uint8_t multiplier);
    static std::vector<uint8_t> prodOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              uint8_t& extender);
    // Calcs quotient of vectors
    static std::vector<uint8_t> quotOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              uint8_t& narrower);

private:
    std::vector<uint8_t> _numIntPart;
    size_t _fractPos;
    bigNumber::Sign _sign;
    bool _decimalPointFlag; // TODO: RM
};

