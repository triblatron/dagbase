//
// Created by Tony Horrobin on 23/11/2025.
//

#include "util/DynamicEnum.h"
#include "core/Variant.h"
#include "core/ConfigurationElement.h"
#include "core/LuaInterface.h"

#include <gtest/gtest.h>

class DynamicEnum_testLookup : public ::testing::TestWithParam<std::tuple<const char*, const char*, std::uint32_t>>
{

};

TEST_P(DynamicEnum_testLookup, testExpectedValue)
{
    auto configStr = std::get<0>(GetParam());
    auto name = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());

    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::DynamicEnum sut;
    sut.configure(*config);
    EXPECT_EQ(value, sut.lookup(dagbase::Atom::intern(name)));
}

INSTANTIATE_TEST_SUITE_P(DynamicEnum, DynamicEnum_testLookup, ::testing::Values(
        std::make_tuple("data/tests/DynamicEnum/Empty.lua", "TEST", 0),
        std::make_tuple("data/tests/DynamicEnum/One.lua", "TEST", 1),
        std::make_tuple("data/tests/DynamicEnum/Multiple.lua", "ONE_BIT TWO_BIT", 3),
        std::make_tuple("etc/keyCodes.lua", "KEY_A", 65)
        ));
