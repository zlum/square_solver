#pragma once

#include <array>
#include <utility>

// Array with coefficients of quadratic equation
using QuadCoeffs = std::array<int, 3>;

// Struct with coefficients and roots of quadratic equation
struct QuadEquation
{
    QuadCoeffs coeffs;
    std::pair<double, double> roots;
};
