#include <gtest/gtest.h>

#include "bignumber/bignumber.h"
#include "bignumber/bignumberbuilder.h"
#include "buffer.hpp"
#include "quadprinter.h"
#include "quadreader.h"
#include "quadsolver.h"

using namespace std;
using namespace bigNumber;

TEST(CharStrInput, BigNumber)
{
    BigNumberBuilder bnb1;
    BigNumberBuilder bnb2;

    bnb1.appendChar('9');
    bnb1.appendChar('8');
    bnb1.appendChar('0');
    bnb1.appendChar('4');
    bnb1.appendChar('1');
    bnb1.appendChar(',');
    bnb1.appendChar('7');
    bnb1.appendChar('9');
    bnb1.appendChar('9');
    bnb1.appendChar('3');
    bnb1.appendChar('0');
    bnb1.appendChar('0');
    bnb1.appendChar('0');

    bnb2.appendStr("98041.7993");

    BigNumber test1{bnb1.build()};
    BigNumber test2{bnb2.build()};

    EXPECT_EQ(test1, test2);
}

TEST(SumDiff, BigNumber)
{
    BigNumberBuilder bnb1;
    BigNumberBuilder bnb2;

    bnb1.appendStr("98041.7993");
    bnb2.appendStr("680.4");

    BigNumber test1{bnb1.build()};
    BigNumber test2{bnb2.build()};

    BigNumber testSum{test1 + test2};

    EXPECT_EQ(test1, testSum - test2);
}

TEST(ProdQuat, BigNumber)
{
    BigNumberBuilder bnb1;
    BigNumberBuilder bnb2;

    bnb1.appendStr("98041.7993");
    bnb2.appendStr("680.4");

    BigNumber test1{bnb1.build()};
    BigNumber test2{bnb2.build()};

    BigNumber testProd{test1 * test2};

    EXPECT_EQ(test1, testProd / test2);
}

TEST(SumProd, BigNumber)
{
    BigNumberBuilder bnb1;
    BigNumberBuilder bnb2;

    bnb1.appendStr("98041.7993");
    bnb2.appendStr("3");

    BigNumber test1{bnb1.build()};
    BigNumber test2{bnb2.build()};

    EXPECT_EQ(test1 + test1 + test1, test1 * test2);
}

TEST(QuatSum, BigNumber)
{
    BigNumberBuilder bnb1;
    BigNumberBuilder bnb2;

    bnb1.appendStr("98041.7993");
    bnb2.appendStr("3");

    BigNumber test1{bnb1.build()};
    BigNumber test2{bnb2.build()};

    BigNumber testQuat{test1 / test2};

    EXPECT_EQ(BigNumber(testQuat + testQuat + testQuat).round(), test1);
}
