//
// Created by Tony Horrobin on 30/10/2025.
//

#include "util/ParameterLookup.h"
#include "core/ConfigurationElement.h"
#include "core/LuaInterface.h"

#include <gtest/gtest.h>

class ParameterLookup_testInterpolateAtom : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

TEST_P(ParameterLookup_testInterpolateAtom, testExpectedValue)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::ConfigurationElement* config = nullptr;
    {
        dagbase::Lua lua;

        config = dagbase::ConfigurationElement::fromFile(lua, configStr);
        ASSERT_NE(nullptr, config);
    }
    dagbase::ParameterLookup sut;
    if (auto element = config->findElement("sut"); element)
    {
        sut.configure(*element);
    }
    else
    {
        FAIL() << "Expected existing sut config";
    }
    dagbase::Atom input;
    dagbase::ConfigurationElement::readConfig(*config, "input", &input);
    auto actual = sut.interpolate(input);
    dagbase::Variant result;
    dagbase::ConfigurationElement::readConfig(*config, "result", &result);
    EXPECT_EQ(result, actual);
}

INSTANTIATE_TEST_SUITE_P(ParameterLookup, ParameterLookup_testInterpolateAtom, ::testing::Values(
        std::make_tuple("data/tests/ParameterLookup/Empty.lua"),
        std::make_tuple("data/tests/ParameterLookup/NoParameters.lua"),
        std::make_tuple("data/tests/ParameterLookup/OneParameterNotFound.lua"),
        std::make_tuple("data/tests/ParameterLookup/OneParameter.lua"),
        std::make_tuple("data/tests/ParameterLookup/TwoParameters.lua"),
        std::make_tuple("data/tests/ParameterLookup/TwoParametersOneNotFound.lua"),
        std::make_tuple("data/tests/ParameterLookup/OneIntegerParameter.lua")
        ));
