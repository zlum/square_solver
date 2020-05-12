#pragma once

namespace bigNumber
{
    enum class Sign: int
    {
        positive,
        negative
    };

    // TODO: Use
    enum class Status: int
    {
        zero,
        nan,
        inf,
        normal
    };

    enum class ComareRes: int
    {
        less,
        equal,
        greater
    };

} // namespace bigNumber
