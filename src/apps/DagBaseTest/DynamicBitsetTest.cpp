//
// Created by Tony Horrobin on 27/11/2025.
//

#include "util/DynamicBitset.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"

#include <gtest/gtest.h>

class DynamicBitset_testBitwiseOp : public ::testing::TestWithParam<std::tuple<const char*>>
{
public:
    void configure(dagbase::ConfigurationElement& config);

    void makeItSo();
protected:
    struct BitwiseOp
    {
        std::string opcode;
        dagbase::Variant operand;
        dagbase::Variant operand2;
        dagbase::Variant result;
        std::string message;

        void configure(dagbase::ConfigurationElement& config);

        void makeItSo(dagbase::DynamicBitset<std::uint32_t>& sut);
    };
    std::vector<BitwiseOp> _ops;
    dagbase::DynamicBitset<std::uint32_t> _sut;
};

void DynamicBitset_testBitwiseOp::BitwiseOp::configure(dagbase::ConfigurationElement &config)
{
    dagbase::ConfigurationElement::readConfig(config, "opcode", &opcode);
    dagbase::ConfigurationElement::readConfig(config, "operand", &operand);
    dagbase::ConfigurationElement::readConfig(config, "operand2", &operand2);
    dagbase::ConfigurationElement::readConfig(config, "result", &result);
    dagbase::ConfigurationElement::readConfig(config, "message", &message);
}

void DynamicBitset_testBitwiseOp::BitwiseOp::makeItSo(dagbase::DynamicBitset<std::uint32_t> &sut)
{
    if (opcode == "PUSH_BACK")
    {
        sut.push_back(operand.asBool());
        EXPECT_EQ(result.asInteger(), sut.size());
    }
    else if (opcode == "TEST_BIT")
    {
        EXPECT_EQ(result.asBool(), sut.testBit(operand.asInteger())) << message;
    }
    else if (opcode == "SET_BIT")
    {
        sut.setBit(operand.asInteger());
    }
    else if (opcode == "CLEAR_BIT")
    {
        sut.clearBit(operand.asInteger());
    }
    else if (opcode == "FLIP_BIT")
    {
        sut.flipBit(operand.asInteger());
    }
    else if (opcode == "NUN_BLOCKS")
    {
        EXPECT_EQ(result.asInteger(), sut.numBlocks());
    }
    else if (opcode == "RESERVE")
    {
        sut.reserve(operand.asInteger());
    }
    else if (opcode == "CAPACITY")
    {
        EXPECT_EQ(result.asInteger(), sut.capacity());
    }
    else if (opcode == "RESIZE")
    {
        sut.resize(operand.asInteger(), operand2.asBool());
    }
    else if (opcode == "SIZE")
    {
        EXPECT_EQ(result.asInteger(), sut.size());
    }
    else if (opcode == "POP_BACK")
    {
        sut.pop_back();
    }
    else if (opcode == "AND")
    {
        decltype(_sut) op2;
        op2.fromString(operand.toString());
        sut &= op2;
    }
    else if (opcode == "OR")
    {
        decltype(_sut) op2;
        op2.fromString(operand.toString());
        sut |= op2;
    }
    else if (opcode == "XOR")
    {
        decltype(_sut) op2;
        op2.fromString(operand.toString());
        sut ^= op2;
    }
    else if (opcode == "TO_STRING")
    {
        std::string actual;
        sut.toString(actual);
        EXPECT_EQ(result.toString(), actual);
    }
    else if (opcode == "ALL")
    {
        EXPECT_EQ(result.asBool(), sut.all());
    }
    else if (opcode == "ANY")
    {
        EXPECT_EQ(result.asBool(), sut.any());
    }
    else if (opcode == "NONE")
    {
        EXPECT_EQ(result.asBool(), sut.none());
    }
    else if (opcode == "COUNT")
    {
        EXPECT_EQ(result.asInteger(), sut.count());
    }
    else if (opcode == "AT")
    {
        EXPECT_EQ(result.asBool(), ((const dagbase::DynamicBitset<std::uint32_t> &)sut).operator[](operand.asInteger()));
    }
    else if (opcode == "ASSIGN")
    {
        sut[operand.asInteger()] = operand2.asBool();
    }
    else if (opcode == "FIND_FIRST")
    {
        auto actual = sut.findFirst();
        if (result.asString() == "NPOS")
        {
            EXPECT_EQ(dagbase::DynamicBitset<std::uint32_t>::npos, actual);
        }
        else
        {
            EXPECT_EQ(result.asInteger(), actual);
        }
    }
    else if (opcode == "FIND_FIRST_FROM")
    {
        auto actual = sut.findFirst(operand.asInteger());
        if (result.asString() == "NPOS")
        {
            EXPECT_EQ(dagbase::DynamicBitset<std::uint32_t>::npos, actual);
        }
        else
        {
            EXPECT_EQ(result.asInteger(), actual);
        }
    }
    else if (opcode == "FIND_NEXT")
    {
        auto actual = sut.findNext(operand.asInteger());
        if (result.asString() == "NPOS")
        {
            EXPECT_EQ(dagbase::DynamicBitset<std::uint32_t>::npos, actual);
        }
        else
        {
            EXPECT_EQ(result.asInteger(), actual);
        }
    }
    else if (opcode == "NUM_BLOCKS")
    {
        EXPECT_EQ(result.asInteger(), sut.numBlocks());
    }
    else
    {
        FAIL() << "Unexpected opcode " << opcode;
    }
}

void DynamicBitset_testBitwiseOp::configure(dagbase::ConfigurationElement &config)
{
    dagbase::ConfigurationElement::readConfigVector(config, "ops", &_ops);
}

void DynamicBitset_testBitwiseOp::makeItSo()
{
    for (auto op : _ops)
    {
        op.makeItSo(_sut);
    }
}

TEST_P(DynamicBitset_testBitwiseOp, testExpectedResult)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    configure(*config);
    makeItSo();
}

INSTANTIATE_TEST_SUITE_P(DynamicBitset, DynamicBitset_testBitwiseOp, ::testing::Values(
        std::make_tuple("data/tests/DynamicBitset/Empty.lua"),
        std::make_tuple("data/tests/DynamicBitset/OneNotSet.lua"),
        std::make_tuple("data/tests/DynamicBitset/OneSet.lua"),
        std::make_tuple("data/tests/DynamicBitset/TwoBits.lua"),
        std::make_tuple("data/tests/DynamicBitset/MultipleBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/SetOne.lua"),
        std::make_tuple("data/tests/DynamicBitset/ClearOne.lua"),
        std::make_tuple("data/tests/DynamicBitset/FlipOff.lua"),
        std::make_tuple("data/tests/DynamicBitset/FlipOn.lua"),
        std::make_tuple("data/tests/DynamicBitset/TwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/ReserveOne.lua"),
        std::make_tuple("data/tests/DynamicBitset/ReserveTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/ResizeOneFalse.lua"),
        std::make_tuple("data/tests/DynamicBitset/ResizeOneTrue.lua"),
        std::make_tuple("data/tests/DynamicBitset/ResizeTwo.lua"),
        std::make_tuple("data/tests/DynamicBitset/ResizeTwice.lua"),
        std::make_tuple("data/tests/DynamicBitset/ResizeTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/PushBackThenResizeTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/ResizeIntermediateBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/ResizeTwiceTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/ResizeDownOneBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/ResizeDownTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/PopBackToEmpty.lua"),
        std::make_tuple("data/tests/DynamicBitset/PopBackWithinBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/PopBackWithinSecondBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/PopBackAcrossBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/AndOneBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/AndTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/OrTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/XorTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/AllFalse.lua"),
        std::make_tuple("data/tests/DynamicBitset/AllFalseTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/AllTrue.lua"),
        std::make_tuple("data/tests/DynamicBitset/AllTrueTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/AnyFalse.lua"),
        std::make_tuple("data/tests/DynamicBitset/AnyFalseTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/AnyTrue.lua"),
        std::make_tuple("data/tests/DynamicBitset/AnyTrueTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/NoneFalse.lua"),
        std::make_tuple("data/tests/DynamicBitset/NoneFalseTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/NoneTrue.lua"),
        std::make_tuple("data/tests/DynamicBitset/NoneTrueTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/CountNone.lua"),
        std::make_tuple("data/tests/DynamicBitset/CountOne.lua"),
        std::make_tuple("data/tests/DynamicBitset/CountTwoAcrossTwoBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/AtFalse.lua"),
        std::make_tuple("data/tests/DynamicBitset/AtTrue.lua"),
        std::make_tuple("data/tests/DynamicBitset/Assign.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindFirstNone.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindFirstInFirstBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindFirstInSecondBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindFirstFromNoneInFirstBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindFirstFromExistingInFirstBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindFirstFromExistingInSecondBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindFirstFromExistingInThirdBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindNextNoneInFirstBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindNextExistingInFirstBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindNextExistingInSecondBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindNextExistingInThirdBlock.lua"),
        std::make_tuple("data/tests/DynamicBitset/FindNextExistingCrossingBlocks.lua")
        ));

class DynamicBitset_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

TEST_P(DynamicBitset_testRoundTrip, testExpectedValue)
{
    auto str = std::get<0>(GetParam());
    dagbase::DynamicBitset<std::uint32_t> sut;
    sut.fromString(str);
    std::string actual;
    sut.toString(actual);
    EXPECT_EQ(str, actual);
}

INSTANTIATE_TEST_SUITE_P(DynamicBitset, DynamicBitset_testRoundTrip, ::testing::Values(
        std::make_tuple("1"),
        std::make_tuple("10"),
        std::make_tuple("100000000000000000000000000000000")
        ));