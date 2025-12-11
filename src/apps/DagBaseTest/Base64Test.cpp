//
// Created by Tony Horrobin on 11/12/2025.
//

#include "util/Base64.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"

#include <gtest/gtest.h>

class Base64_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

TEST_P(Base64_testRoundTrip, testRoundTrip)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    std::string input;
    dagbase::ConfigurationElement::readConfig(*config, "input", &input);
    std::string outputStr;
    std::vector<std::uint8_t> output;
    dagbase::ConfigurationElement::readConfig(*config, "output", &outputStr);
    output.reserve(outputStr.length());
    for (std::size_t i=0; i<outputStr.length(); ++i)
    {
        output.emplace_back(outputStr[i]);
    }
    std::vector<std::uint8_t> actualOutput;
    dagbase::base64encode((const std::uint8_t*)input.c_str(), input.length(), &actualOutput);
    EXPECT_EQ(output, actualOutput);
    std::vector<std::uint8_t> actualDecodedOutput;
    dagbase::base64decode(output.data(), output.size(), &actualDecodedOutput);
    std::vector<std::uint8_t> inputBuf;
    inputBuf.reserve(input.length());
    for (std::size_t i=0; i<input.length(); ++i)
    {
        inputBuf.emplace_back(input[i]);
    }
    EXPECT_EQ(inputBuf, actualDecodedOutput);
}

INSTANTIATE_TEST_SUITE_P(Base64, Base64_testRoundTrip, ::testing::Values(
        std::make_tuple("data/tests/Base64/NoPadding.lua")
        ));