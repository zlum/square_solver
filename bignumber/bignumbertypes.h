#pragma once

#include <limits>
#include <stdexcept>

namespace bigNumber
{
    class OverflowException: public std::exception{};

    static constexpr int MAX_SIZE = std::numeric_limits<int>::max();

    enum class Sign: int
    {
        positive,
        negative
    };

    enum class Status: int
    {
        inf,
        nan,
        normal
    };
} // namespace bigNumber
