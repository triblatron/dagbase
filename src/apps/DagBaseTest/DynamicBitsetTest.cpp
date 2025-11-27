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
        dagbase::Variant result;

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
    dagbase::ConfigurationElement::readConfig(config, "result", &result);
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
        EXPECT_EQ(result.asBool(), sut.testBit(operand.asInteger()));
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
        std::make_tuple("data/tests/DynamicBitset/MultipleBlocks.lua")
        ));