#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace numVector
{
    // Calc sum of vectors. Additional digit will be written to (carry)
    std::vector<uint8_t> sumOfVectors(const std::vector<uint8_t>& lNum,
                                             const std::vector<uint8_t>& rNum,
                                             uint8_t& carry,
                                             size_t lShift,
                                             size_t rShift);

    // Calcs difference of vectors. Additional digit will be written to (carry)
    std::vector<uint8_t> diffOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              uint8_t& carry,
                                              size_t lShift,
                                              size_t rShift);

    // Calcs product of vectors. Additional digit will be written to (carry)
    std::vector<uint8_t> prodOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              uint8_t& carry);

    std::vector<uint8_t> prodHelperMultiply(const std::vector<uint8_t>& lNum,
                                                   uint8_t multiplier);

    // Calcs quotient of vectors
    // Decimal point position will be written to (carry)
    std::vector<uint8_t> quotOfVectors(const std::vector<uint8_t>& lNum,
                                              const std::vector<uint8_t>& rNum,
                                              size_t lShift,
                                              size_t rShift,
                                              size_t precision,
                                              size_t& decPos);

    bool quotHelperLess(const std::vector<uint8_t>& lNum,
                               const std::vector<uint8_t>& rNum);

    void quotHelperSubtract(std::vector<uint8_t>& lNum,
                                   const std::vector<uint8_t>& rNum,
                                   size_t rShift);

    bool compareOfVectors(const std::vector<uint8_t>& lNum,
                                 size_t lFractPos,
                                 const std::vector<uint8_t>& rNum,
                                 size_t rFractPos,
                                 const std::function<bool(int, int)>& functor);

    // nonsignificant digit
    void popZeroes(std::vector<uint8_t>& vec);
    // Find first non-zero digit from the end of number to begin
    size_t trackZeroes(const std::vector<uint8_t>& vec, size_t pos);
} // namespace numVector
