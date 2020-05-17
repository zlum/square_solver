#pragma once

#include "bignumber.h"
#include "bignumbertypes.h"

#include <string>

class BigNumberBuilder
{
public:
    explicit BigNumberBuilder();
    virtual ~BigNumberBuilder() = default;

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
    BigNumber _dummyNum;
    bool _decimalPointFlag;
    bool _empty;
};
