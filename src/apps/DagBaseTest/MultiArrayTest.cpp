//
// Created by Tony Horrobin on 25/09/2025.
//

#include "util/MultiArray.h"
#include "util/DynamicMultiArray.h"
#include "core/ConfigurationElement.h"
#include "core/LuaInterface.h"
#include "test/TestUtils.h"

#include <gtest/gtest.h>

class MultiArray_testEmplaceBack : public ::testing::TestWithParam<std::tuple<int, double, std::string>>
{

};

TEST_P(MultiArray_testEmplaceBack, testExpectedValues)
{
    dagbase::MultiArray<int, double, std::string> sut;
    auto i = std::get<0>(GetParam());
    auto d = std::get<1>(GetParam());
    auto s = std::get<2>(GetParam());
    sut.reserve(1);
    sut.emplace_back(i,d,s);
    EXPECT_EQ(i, sut.array<0>()[0]);
    EXPECT_EQ(d, sut.array<1>()[0]);
    EXPECT_EQ(s, sut.array<2>()[0]);
}

INSTANTIATE_TEST_SUITE_P(MultiArray, MultiArray_testEmplaceBack, ::testing::Values(
        std::make_tuple(1, 2.5, "test")
        ));

class DynamicMultiArray_testConfigure : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(DynamicMultiArray_testConfigure, testExpectedValue)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::DynamicMultiArray sut;
    sut.configure(*config);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actualValue = sut.find(path);
    assertComparison(value, actualValue, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(DynamicMultiArray, DynamicMultiArray_testConfigure, ::testing::Values(
        std::make_tuple("data/tests/DynamicMultiArray/MultipleArrays.lua", "arrays[0].items[0]", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/DynamicMultiArray/MultipleArrays.lua", "arrays[1].items[0]", 2.5, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/DynamicMultiArray/MultipleArrays.lua", "arrays[2].items[0]", std::string("test"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/DynamicMultiArray/MultipleArrays.lua", "arrays[3].items[0]", false, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/DynamicMultiArray/MultipleArrays.lua", "arrays[3].items[1]", true, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/DynamicMultiArray/MultipleArrays.lua", "arrays[4].items[0]", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));