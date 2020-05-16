#pragma once

#include "bignumber/bignumber.h"

#include <array>
#include <utility>

// Array with coefficients of quadratic equation
using QuadCoeffs = std::array<BigNumber, 3>;
using QuadRoots = std::pair<BigNumber, BigNumber>;

// Struct with coefficients and roots of quadratic equation
struct QuadEquation
{
    QuadCoeffs coeffs;
    QuadRoots roots;
};
