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
    BigNumberBuilder bnb;

    bnb.appendChar('9');
    bnb.appendChar('8');
    bnb.appendChar('0');
    bnb.appendChar('4');
    bnb.appendChar('1');
    bnb.appendChar(',');
    bnb.appendChar('7');
    bnb.appendChar('9');
    bnb.appendChar('9');
    bnb.appendChar('3');
    bnb.appendChar('0');
    bnb.appendChar('0');
    bnb.appendChar('0');
    BigNumber test1{bnb.build()};

    bnb.appendStr("98041.7993");
    BigNumber test2{bnb.build()};

    EXPECT_EQ(test1, test2);
}

TEST(SumDiff, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("98041.7993");
    BigNumber test1{bnb.build()};

    bnb.appendStr("680.4");
    BigNumber test2{bnb.build()};

    BigNumber testSum{test1 + test2};

    EXPECT_EQ(test1, testSum - test2);
}

TEST(ProdQuat, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("98041.7993");
    BigNumber test1{bnb.build()};

    bnb.appendStr("680.4");
    BigNumber test2{bnb.build()};

    BigNumber testProd{test1 * test2};

    EXPECT_EQ(test1, testProd / test2);
}

TEST(SumProd, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("98041.7993");
    BigNumber test1{bnb.build()};

    bnb.appendStr("3");
    BigNumber test2{bnb.build()};

    EXPECT_EQ(test1 + test1 + test1, test1 * test2);
}

TEST(BigIntegerSum, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("99999999999999999999999999999999999999999999999999999");
    BigNumber test1{bnb.build()};

    bnb.appendStr("1");
    BigNumber test2{bnb.build()};

    bnb.appendStr("100000000000000000000000000000000000000000000000000000");
    BigNumber test3{bnb.build()};

    EXPECT_EQ(test1 + test2, test3);
}

TEST(BigFractialSum, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("0.99999999999999999999999999999999999999999999999999999");
    BigNumber test1{bnb.build()};

    bnb.appendStr("0.00000000000000000000000000000000000000000000000000001");
    BigNumber test2{bnb.build()};

    bnb.appendStr("1");
    BigNumber test3{bnb.build()};

    EXPECT_EQ(test1 + test2, test3);
}

TEST(BigFractialRound, BigNumber)
{
    // Precision = 30
    BigNumberBuilder bnb;

    bnb.appendStr("37598041.799299999999999999999999999999999999999999999998");
    BigNumber test1{bnb.build()};

    bnb.appendStr("37598041.7993");
    BigNumber test2{bnb.build()};

    bnb.appendStr("11.234562222222222222222222222223");
    BigNumber test3{bnb.build()};

    bnb.appendStr("11.2345622222222222222222222222234");
    BigNumber test4{bnb.build()};

    EXPECT_EQ(test1.round(), test2);
    EXPECT_EQ(test1.round(), test1.round());
    EXPECT_EQ(test3.round(), test4.round());
}

TEST(Less, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("37598041.799299999999999999999999999999999999999999999998");
    BigNumber test1{bnb.build()};

    bnb.appendStr("37598041.7993");
    BigNumber test2{bnb.build()};

    bnb.appendStr("7993");
    BigNumber test3{bnb.build()};
    bnb.appendStr("799299999999999999999999999999999999999999999998");
    BigNumber test4{bnb.build()};

    bnb.appendStr("0.0000000000000000000000000000000000000000007993");
    BigNumber test5{bnb.build()};
    bnb.appendStr("799299999999999999999999999999999999999999999998");
    BigNumber test6{bnb.build()};

    EXPECT_TRUE(test1 < test2);
    EXPECT_TRUE(test3 < test4);
    EXPECT_TRUE(test5 < test6);
}

TEST(Greater, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("37598041.799299999999999999999999999999999999999999999998");
    BigNumber test1{bnb.build()};

    bnb.appendStr("37598041.7993");
    BigNumber test2{bnb.build()};

    bnb.appendStr("7993");
    BigNumber test3{bnb.build()};
    bnb.appendStr("799299999999999999999999999999999999999999999998");
    BigNumber test4{bnb.build()};

    bnb.appendStr("0.0000000000000000000000000000000000000000007993");
    BigNumber test5{bnb.build()};
    bnb.appendStr("799299999999999999999999999999999999999999999998");
    BigNumber test6{bnb.build()};

    EXPECT_TRUE(test2 > test1);
    EXPECT_TRUE(test4 > test3);
    EXPECT_TRUE(test6 > test5);
}

TEST(QuatSum, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("98041.7993");
    BigNumber test1{bnb.build()};

    bnb.appendStr("3");
    BigNumber test2{bnb.build()};

    BigNumber testQuat{test1 / test2};

    EXPECT_EQ(BigNumber(testQuat + testQuat + testQuat).round(), test1);
}

TEST(SqrtQuat, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("1600000000000000000000");
    BigNumber test1{bnb.build()};

    bnb.appendStr("40000000000");
    BigNumber test2{bnb.build()};

    EXPECT_EQ(BigNumber(test1.sqrt()).round(), test1 / test2);
}

TEST(Zero, BigNumber)
{
    BigNumberBuilder bnb;

    BigNumber test1{bnb.build()};

    bnb.appendChar('0');
    BigNumber test2{bnb.build()};

    bnb.appendStr("000000000000000000000000000000000000000000000000000");
    BigNumber test3{bnb.build()};

    bnb.appendStr("000000000000000000.000000000000000000000000000000000");
    BigNumber test4{bnb.build()};

    EXPECT_TRUE(test1.isZero());
    EXPECT_TRUE(test2.isZero());
    EXPECT_TRUE(test3.isZero());
    EXPECT_TRUE(test4.isZero());
    EXPECT_EQ(test1, test2);
    EXPECT_EQ(test2, test3);
    EXPECT_EQ(test3, test4);
}

TEST(NegativeSum, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("680.4");
    BigNumber test1{bnb.build()};

    bnb.appendStr("98041.7993");
    BigNumber test2{bnb.build()};

    EXPECT_EQ(BigNumber{test1 + test2}, BigNumber{-test1 + -test2});
}

TEST(NegativeDiff, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("680.4");
    BigNumber test1{bnb.build()};

    bnb.appendStr("98041.7993");
    BigNumber test2{bnb.build()};

    EXPECT_EQ(BigNumber{test1 - test2}, BigNumber{test1 + -test2});
}

TEST(NegativeInput, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("-344358550.052354");
    BigNumber test1{bnb.build()};

    bnb.appendStr("344358550.052354");
    BigNumber test2{bnb.build()};

    EXPECT_EQ(test1, -test2);
}

TEST(ConcreteSum, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("1");
    BigNumber test1{bnb.build()};

    bnb.appendStr("2");
    BigNumber test2{bnb.build()};

    EXPECT_EQ(test1 + -test2, -test1);
    EXPECT_EQ(-test1 + test2, test1);
    EXPECT_EQ(-test1 + -test1, -test2);
}

TEST(ConcreteDiff, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("0");
    BigNumber test0{bnb.build()};

    bnb.appendStr("1");
    BigNumber test1{bnb.build()};

    bnb.appendStr("2");
    BigNumber test2{bnb.build()};

    bnb.appendStr("3");
    BigNumber test3{bnb.build()};

    EXPECT_EQ(test1 - -test2, test3);
    EXPECT_EQ(-test1 - test2, -test3);
    EXPECT_EQ(-test1 - -test1, -test0);
}
