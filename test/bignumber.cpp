#include <gtest/gtest.h>

#include "bignumber/bignumber.h"
#include "bignumber/bignumberbuilder.h"

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

    bnb.appendStr("2");
    BigNumber test3{bnb.build()};

    bnb.appendStr("25674.5");
    BigNumber test4{bnb.build()};

    bnb.appendStr("51349");
    BigNumber test5{bnb.build()};

    bnb.appendStr("25674.0000005");
    BigNumber test6{bnb.build()};

    bnb.appendStr("51348.000001");
    BigNumber test7{bnb.build()};

    BigNumber testProd1{test1 * test2};
    BigNumber testProd2{test3 * test4};
    BigNumber testProd3{test3 * test6};

    EXPECT_EQ(test1, testProd1 / test2);
    EXPECT_EQ(test3, testProd2 / test4);
    EXPECT_EQ(test7, testProd3);
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

    bnb.appendStr("0.0000000000000000000000000000000000000000000000000000002");
    BigNumber test5{bnb.build()};

    bnb.appendStr("0");
    BigNumber test6{bnb.build()};

    EXPECT_EQ(test1.round(), test2);
    EXPECT_EQ(test1.round(), test1.round());
    EXPECT_EQ(test3.round(), test4.round());
    EXPECT_EQ(test5.round(), test6);
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

    BigNumber BigPrec{{1}, 31, Sign::positive, Status::normal};
    BigNumber Big1{{1}, 0, Sign::positive, Status::normal};

    EXPECT_TRUE(test1 < test2);
    EXPECT_TRUE(test3 < test4);
    EXPECT_TRUE(test5 < test6);
    EXPECT_FALSE(Big1 - BigPrec < BigPrec);
    EXPECT_TRUE(Big1 - BigPrec < Big1);
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

    BigNumber BigPrec{{1}, 31, Sign::positive, Status::normal};
    BigNumber Big1{{1}, 0, Sign::positive, Status::normal};

    EXPECT_TRUE(test2 > test1);
    EXPECT_TRUE(test4 > test3);
    EXPECT_TRUE(test6 > test5);
    EXPECT_TRUE(Big1 - BigPrec > BigPrec);
    EXPECT_TRUE(Big1 + BigPrec > Big1);
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

    bnb.appendStr("-0");
    BigNumber test5{bnb.build()};

    EXPECT_TRUE(test1.isZero());
    EXPECT_TRUE(test2.isZero());
    EXPECT_TRUE(test3.isZero());
    EXPECT_TRUE(test4.isZero());
    EXPECT_EQ(test1, test2);
    EXPECT_EQ(test2, test3);
    EXPECT_EQ(test3, test4);
    EXPECT_EQ(test4, test5);
    EXPECT_FALSE(test4 > test5);
    EXPECT_FALSE(test4 < test5);
    EXPECT_FALSE(test5 > test4);
    EXPECT_FALSE(test5 < test4);
}

TEST(NegativeSum, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("680.4");
    BigNumber test1{bnb.build()};

    bnb.appendStr("98041.7993");
    BigNumber test2{bnb.build()};

    EXPECT_EQ(BigNumber{test1 + test2}, -BigNumber{-test1 + -test2});
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

    bnb.appendStr("999999999999999999999999999999999999999999999999999999");
    BigNumber testMany9{bnb.build()};

    bnb.appendStr("1000000000000000000000000000000000000000000000000000000");
    BigNumber testMany9and1{bnb.build()};

    EXPECT_EQ(test1 - -test2, test3);
    EXPECT_EQ(-test1 - test2, -test3);
    EXPECT_EQ(-test1 - -test1, -test0);
    EXPECT_EQ(testMany9and1 - test1, testMany9);
    EXPECT_EQ(testMany9 - testMany9and1, -test1);
}

TEST(ConcreteQuat, BigNumber)
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

    bnb.appendStr("99999999999999999999999999999999999999999999999999999999");
    BigNumber testMany9{bnb.build()};

    bnb.appendStr("49999999999999999999999999999999999999999999999999999999.5");
    BigNumber testHalfOfMany9{bnb.build()};

    EXPECT_EQ(testMany9 / test2, testHalfOfMany9);
//    EXPECT_EQ(-test1 - test2, -test3);
//    EXPECT_EQ(-test1 - -test1, -test0);
//    EXPECT_EQ(testMany9and1 - test1, testMany9);
//    EXPECT_EQ(testMany9 - testMany9and1, -test1);
}

TEST(FractialInput, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("1");
    BigNumber test1{bnb.build()};

    bnb.appendStr("200");
    BigNumber test2{bnb.build()};

    bnb.appendStr("0.005");
    BigNumber test3{bnb.build()};

    EXPECT_EQ(test1 / test2, test3);
}

TEST(ZeroNum, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("");
    BigNumber test0{bnb.build()};

    bnb.appendStr("0");
    BigNumber test1{bnb.build()};

    bnb.appendStr("0.0");
    BigNumber test2{bnb.build()};

    bnb.appendStr("000");
    BigNumber test3{bnb.build()};

    bnb.appendStr("-0.1");
    BigNumber testNotZero{bnb.build()};

    EXPECT_EQ(test0, test1);
    EXPECT_EQ(test1, test2);
    EXPECT_EQ(test2, test3);

    EXPECT_EQ(test0 + testNotZero, testNotZero);
    EXPECT_EQ(test1 + testNotZero, testNotZero);
    EXPECT_EQ(test2 + testNotZero, testNotZero);
    EXPECT_EQ(test3 + testNotZero, testNotZero);
}

TEST(Equal, BigNumber)
{
    BigNumberBuilder bnb;

    BigNumber testPosInf{{}, 0, Sign::positive, Status::inf};
    BigNumber testNegInf{{}, 0, Sign::negative, Status::inf};
    BigNumber testPosNaN{{}, 0, Sign::positive, Status::nan};
    BigNumber testNegNaN{{}, 0, Sign::negative, Status::nan};
    BigNumber testPosZero{{}, 0, Sign::positive, Status::normal};
    BigNumber testNegZero{{}, 0, Sign::negative, Status::normal};

    bnb.appendStr("0");
    BigNumber testZero{bnb.build()};

    bnb.appendStr("000000");
    BigNumber testZeroes{bnb.build()};

    bnb.appendStr(".0");
    BigNumber testDotZero{bnb.build()};

    bnb.appendStr("00000000.0");
    BigNumber testZeroesDot{bnb.build()};

    bnb.appendStr("0.000000000000000000");
    BigNumber testDotZeroes{bnb.build()};

    bnb.appendStr("000.00000000000");
    BigNumber testZeroesDotZeroes{bnb.build()};

    EXPECT_EQ(testPosInf, testPosInf);
    EXPECT_NE(testPosInf, testNegInf);
    EXPECT_NE(testPosNaN, testPosNaN);
    EXPECT_NE(testPosNaN, testNegNaN);
    EXPECT_NE(testNegNaN, testNegNaN);
    EXPECT_EQ(testPosZero, testPosZero);
    EXPECT_EQ(testPosZero, testNegZero);
    EXPECT_EQ(testNegZero, testNegZero);

    EXPECT_EQ(testNegZero, testZero);
    EXPECT_EQ(testZero, testZeroes);
    EXPECT_EQ(testZeroes, testDotZero);
    EXPECT_EQ(testDotZero, testZeroesDot);
    EXPECT_EQ(testZeroesDot, testDotZeroes);
    EXPECT_EQ(testDotZeroes, testZeroesDotZeroes);
    EXPECT_EQ(testZeroesDotZeroes, testZeroesDotZeroes);
}

TEST(DecimalComparison, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("100");
    BigNumber testHundred{bnb.build()};

    bnb.appendStr("0.01");
    BigNumber testSmall{bnb.build()};

    bnb.appendStr("0.0001");
    BigNumber testSmaller{bnb.build()};

    BigNumber testProd = testHundred * testSmaller;

    EXPECT_EQ(testProd, testSmall);
    EXPECT_FALSE(testProd < testSmall);
    EXPECT_FALSE(testProd > testSmall);
    EXPECT_FALSE(testSmall < testProd);
    EXPECT_FALSE(testSmall > testProd);
}

TEST(SumSign, BigNumber)
{
    BigNumberBuilder bnb;
    static const BigNumber Big0{{}, 0, Sign::positive, Status::normal};

    bnb.appendStr("123");
    BigNumber test123{bnb.build()};

    bnb.appendStr("987");
    BigNumber test987{bnb.build()};

    EXPECT_TRUE((-test987 + test123) < Big0);
    EXPECT_TRUE((-test987 + -test123) < Big0);
    EXPECT_TRUE((test123 + -test987) < Big0);
    EXPECT_TRUE((-test123 + -test987) < Big0);
    EXPECT_TRUE(Big0 > (-test987 + test123));
    EXPECT_TRUE(Big0 > (-test987 + -test123));
    EXPECT_TRUE(Big0 > (test123 + -test987));
    EXPECT_TRUE(Big0 > (-test123 + -test987));
}

TEST(DiffSign, BigNumber)
{
    BigNumberBuilder bnb;
    static const BigNumber Big0{{}, 0, Sign::positive, Status::normal};

    bnb.appendStr("123");
    BigNumber test123{bnb.build()};

    bnb.appendStr("987");
    BigNumber test987{bnb.build()};

    EXPECT_TRUE((-test123 - test987) < Big0);
    EXPECT_TRUE((test123 - test987) < Big0);
    EXPECT_TRUE(Big0 > (-test123 - test987));
    EXPECT_TRUE(Big0 > (test123 - test987));
}

TEST(ProdSign, BigNumber)
{
    BigNumberBuilder bnb;
    static const BigNumber Big0{{}, 0, Sign::positive, Status::normal};

    bnb.appendStr("358550.0523");
    BigNumber testPos{bnb.build()};

    bnb.appendStr("-6876.134");
    BigNumber testNeg{bnb.build()};

    EXPECT_TRUE((testPos * testPos) > Big0);
    EXPECT_TRUE((testPos * testNeg) < Big0);
    EXPECT_TRUE((testNeg * testPos) < Big0);
    EXPECT_TRUE((testNeg * testNeg) > Big0);
}

TEST(QuatSign, BigNumber)
{
    BigNumberBuilder bnb;
    static const BigNumber Big0{{}, 0, Sign::positive, Status::normal};

    bnb.appendStr("358550.0523");
    BigNumber testPos{bnb.build()};

    bnb.appendStr("-6876.134");
    BigNumber testNeg{bnb.build()};

    EXPECT_TRUE((testPos / testPos) > Big0);
    EXPECT_TRUE((testPos / testNeg) < Big0);
    EXPECT_TRUE((testNeg / testPos) < Big0);
    EXPECT_TRUE((testNeg / testNeg) > Big0);
}

TEST(SqrtSign, BigNumber)
{
    BigNumberBuilder bnb;
    static const BigNumber Big0{{}, 0, Sign::positive, Status::normal};

    bnb.appendStr("400");
    BigNumber testPos{bnb.build()};
    testPos = testPos.sqrt();

    bnb.appendStr("0");
    BigNumber testZero{bnb.build()};
    testZero = testZero.sqrt();

    bnb.appendStr("-1600");
    BigNumber testNeg{bnb.build()};
    testNeg = testNeg.sqrt();

    EXPECT_TRUE(testPos > Big0);
    EXPECT_TRUE(testZero.getStatus() == Status::nan);
    EXPECT_TRUE(testNeg.getStatus() == Status::nan);
}

TEST(ZeroSign, BigNumber)
{
    BigNumberBuilder bnb;

    bnb.appendStr("0");
    BigNumber testPos{bnb.build()};

    bnb.appendStr("-0");
    BigNumber testNeg{bnb.build()};

    EXPECT_TRUE(testPos == testPos);
    EXPECT_TRUE(testPos == testNeg);
    EXPECT_TRUE(testNeg == testNeg);
}

TEST(SqrtGrow, BigNumber)
{
    BigNumberBuilder bnb;
    static const BigNumber Big2{{2}, 0, Sign::positive, Status::normal};
    static const BigNumber Big4{{4}, 0, Sign::positive, Status::normal};

    bnb.appendStr("4");
    BigNumber test1{bnb.build()};

    bnb.appendStr("2");
    BigNumber test2{bnb.build()};

    for(int i = 0; i < 4; ++i)
    {
        test1 = test1 * Big4;
        test2 = test2 * Big2;

        EXPECT_EQ(BigNumber(test1.sqrt()).round(), test2);
    }
}
