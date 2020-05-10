#pragma once

#include "bignumber.h"
#include "bignumbertypes.h"

#include <cstdint>
#include <vector>

class BigNumberBuilder
{
public:
    explicit BigNumberBuilder();
//    virtual ~BigNumberBuilder();

    bool append(char symbol);
    BigNumber build();

private:
    void reset();

private:
    std::vector<uint8_t> _numIntPart;
    size_t _fractPos;
    bigNumber::Sign _sign;
    bool _decimalPointFlag;
};
