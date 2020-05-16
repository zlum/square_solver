#pragma once

namespace bigNumber
{
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
