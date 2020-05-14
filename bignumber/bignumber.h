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
            bigNumber::Sign sign,
            bigNumber::Status status); // TODO: Rework

    bool isZero() const;

    bigNumber::Sign getSign() const;
    bigNumber::Status getStatus() const;

    BigNumber sqrt() const;
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

    friend std::ostream& operator <<(std::ostream& os, const BigNumber& num);

private:
    static BigNumber sum(const BigNumber& leftNum, const BigNumber& rightNum);
    static BigNumber diff(const BigNumber& leftNum, const BigNumber& rightNum);

    void setSign(bigNumber::Sign sign); // TODO: RM?
    // Removes nonsignificant digit
    static void popZeroes(std::vector<uint8_t>& vec);

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
    // TODO: Do smt
    static std::vector<uint8_t> diffOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              uint32_t lShift,
                                              uint32_t rShift);
    // Calcs product of vectors
    static std::vector<uint8_t> prodHelper(const std::vector<uint8_t>& lNum,
                                           uint8_t multiplier);
    static std::vector<uint8_t> prodOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              uint8_t& extender);

    static size_t trackZeroes(const std::vector<uint8_t>& vec, size_t pos);
    // Calcs quotient of vectors
    static std::vector<uint8_t> quotOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              size_t lUp,
                                              size_t rUp, // TODO: Fix naming
                                              uint8_t& narrower);

    // TODO: Merge this two isVec* and rename
    static bool isVectorGreater(const std::vector<uint8_t>& lNum,
                                size_t lFractPos,
                                const std::vector<uint8_t>& rNum,
                                size_t rFractPos);

    static bool isVectorLesser(const std::vector<uint8_t>& lNum,
                               size_t lFractPos,
                               const std::vector<uint8_t>& rNum,
                               size_t rFractPos);

    static bool isVectorLesser(const std::vector<uint8_t>& lNum,
                               const std::vector<uint8_t>& rNum);

private:
    std::vector<uint8_t> _numIntPart;
    size_t _fractPos;
    bigNumber::Sign _sign;
    bigNumber::Status _status;
    bool _decimalPointFlag; // TODO: RM
};
