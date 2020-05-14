#pragma once

#include "bignumber.h"
#include "bignumbertypes.h"

#include <cstdint>
#include <string>
#include <vector>

class BigNumberBuilder
{
public:
    explicit BigNumberBuilder();
//    virtual ~BigNumberBuilder();

    // Add a character to the end of number if it is possible
    bool appendChar(char symbol);
    // Add a string from pos to the end of num while it is possible
    size_t appendStr(const std::string& str, size_t pos = 0);
    // Build a BigNumber object and reset own data to default
    BigNumber build();
    // Reset own data to default
    void clear();
    // BigNumber is empty if there were no valid input
    bool isEmpty() const;

private:
    std::vector<uint8_t> _numIntPart;
    size_t _fractPos;
    bigNumber::Sign _sign;
    bool _decimalPointFlag;
    bool _empty;
};
