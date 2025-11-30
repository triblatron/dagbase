//
// Created by Tony Horrobin on 27/11/2025.
//

#include "util/DynamicBitset.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"

#include <gtest/gtest.h>

class DynamicBitSet_testBitwiseOp : public ::testing::TestWithParam<std::tuple<const char*>>
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

void DynamicBitSet_testBitwiseOp::BitwiseOp::configure(dagbase::ConfigurationElement &config)
{
    dagbase::ConfigurationElement::readConfig(config, "opcode", &opcode);
    dagbase::ConfigurationElement::readConfig(config, "operand", &operand);
    dagbase::ConfigurationElement::readConfig(config, "operand2", &operand2);
    dagbase::ConfigurationElement::readConfig(config, "result", &result);
    dagbase::ConfigurationElement::readConfig(config, "message", &message);
}

void DynamicBitSet_testBitwiseOp::BitwiseOp::makeItSo(dagbase::DynamicBitset<std::uint32_t> &sut)
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
}

void DynamicBitSet_testBitwiseOp::configure(dagbase::ConfigurationElement &config)
{
    dagbase::ConfigurationElement::readConfigVector(config, "ops", &_ops);
}

void DynamicBitSet_testBitwiseOp::makeItSo()
{
    for (auto op : _ops)
    {
        op.makeItSo(_sut);
    }
}

TEST_P(DynamicBitSet_testBitwiseOp, testExpectedResult)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    configure(*config);
    makeItSo();
}

INSTANTIATE_TEST_SUITE_P(DynamicBitset, DynamicBitSet_testBitwiseOp, ::testing::Values(
        std::make_tuple("data/tests/DynamicBitset/Empty.lua"),
        std::make_tuple("data/tests/DynamicBitset/OneNotSet.lua"),
        std::make_tuple("data/tests/DynamicBitset/OneSet.lua"),
        std::make_tuple("data/tests/DynamicBitset/TwoBits.lua"),
        std::make_tuple("data/tests/DynamicBitset/MultipleBlocks.lua"),
        std::make_tuple("data/tests/DynamicBitset/SetOne.lua"),
        std::make_tuple("data/tests/DynamicBitset/clearOne.lua"),
        std::make_tuple("data/tests/DynamicBitset/flipOff.lua"),
        std::make_tuple("data/tests/DynamicBitset/flipOn.lua"),
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
        std::make_tuple("data/tests/DynamicBitset/PopBackAcrossBlocks.lua")
        ));