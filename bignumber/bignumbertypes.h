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

    enum class ComareRes: int
    {
        less,
        equal,
        greater
    };

} // namespace bigNumber
