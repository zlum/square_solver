#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace numVector
{
    using NumVectorElement = uint8_t;
    using NumVector = std::vector<NumVectorElement>;

    // Calc sum of vectors. Additional digit will be written to (carry)
    NumVector sumOfVectors(const NumVector& lNum,
                                             const NumVector& rNum,
                                             uint8_t& carry,
                                             size_t lShift,
                                             size_t rShift);

    // Calcs difference of vectors. Additional digit will be written to (carry)
    NumVector diffOfVectors(const NumVector& lNum,
                                              const NumVector& rNum,
                                              uint8_t& carry,
                                              size_t lShift,
                                              size_t rShift);

    // Calcs product of vectors. Additional digit will be written to (carry)
    NumVector prodOfVectors(const NumVector& lNum,
                                              const NumVector& rNum,
                                              uint8_t& carry);

    NumVector prodHelperMultiply(const NumVector& lNum,
                                                   uint8_t multiplier);

    // Calcs quotient of vectors
    // Decimal point position will be written to (carry)
    NumVector quotOfVectors(const NumVector& lNum,
                                              const NumVector& rNum,
                                              size_t lShift,
                                              size_t rShift,
                                              size_t precision,
                                              size_t& decPos);

    bool quotHelperLess(const NumVector& lNum,
                               const NumVector& rNum);

    void quotHelperSubtract(NumVector& lNum,
                                   const NumVector& rNum,
                                   size_t rShift);

    bool compareOfVectors(const NumVector& lNum,
                                 size_t lFractPos,
                                 const NumVector& rNum,
                                 size_t rFractPos,
                                 const std::function<bool(int, int)>& functor);

    // nonsignificant digit
    void popZeroes(NumVector& vec);
    // Find first non-zero digit from the end of number to begin
    size_t trackZeroes(const NumVector& vec, size_t pos);
} // namespace numVector
