//
// Created by Tony Horrobin on 13/12/2025.
//

#include "core/Signal.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"
#include "core/Function.h"

#include <gtest/gtest.h>

class SignalSlot_testInvoke : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

struct TestSlot
{
    TestSlot() = default;;

    void configure(dagbase::ConfigurationElement &config)
    {
        dagbase::ConfigurationElement::readConfig(config, "result", &expectedResult);
    }

    dagbase::Variant operator()()
    {
        result = expectedResult;
        return result;
    }

    void makeItSo()
    {
        EXPECT_EQ(expectedResult, result);
    }

    dagbase::Variant expectedResult;
    dagbase::Variant result;
};

TEST_P(SignalSlot_testInvoke, testExpectedResult)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::Signal<dagbase::Variant()> testSignal;
    TestSlot testSlot = {};
    testSlot.configure(*config);
    testSignal.connect([&testSlot]() {
        return testSlot();
    });

    auto actualResult = testSignal();
    EXPECT_EQ(testSlot.expectedResult, actualResult);
}

INSTANTIATE_TEST_SUITE_P(SignalSlot, SignalSlot_testInvoke, ::testing::Values(
    std::make_tuple("data/tests/SignalSlot/Simple.lua")
    ));

class SignalSlot_testLuaSlot : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

struct LuaSlot
{
    void configure(dagbase::ConfigurationElement &config)
    {
        dagbase::Variant value;
        dagbase::ConfigurationElement::readConfig(config, "slot", &value);
        _func = value.asFunction(nullptr);
    }

    ~LuaSlot()
    {
        delete _func;
    }

    void operator()()
    {
        if (_func) {
            (*_func)(0,0);
        }
    }

    dagbase::Function* _func{nullptr};
};

TEST_P(SignalSlot_testLuaSlot, testExpectedResult)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    LuaSlot testSlot = {};
    testSlot.configure(*config);
    testSlot();
    EXPECT_TRUE(lua.tableExists("result"));
}

INSTANTIATE_TEST_SUITE_P(SignalSlot, SignalSlot_testLuaSlot, ::testing::Values(
    std::make_tuple("data/tests/SignalSlot/LuaSlot.lua")
    ));