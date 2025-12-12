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
        std::make_tuple("data/tests/Base64/NoPadding.lua"),
        std::make_tuple("data/tests/Base64/OnePadding.lua"),
        std::make_tuple("data/tests/Base64/TwoPadding.lua"),
        std::make_tuple("data/tests/Base64/TwoPaddingMultipleGroups.lua"),
        std::make_tuple("data/tests/Base64/OnePaddingMultipleGroups.lua"),
        std::make_tuple("data/tests/Base64/OnePaddingMultipleGroups.lua"),
        std::make_tuple("data/tests/Base64/HairyMultipleGroups.lua"),
        std::make_tuple("data/tests/Base64/RepeatingBlock.lua"),
        std::make_tuple("data/tests/Base64/LuaFunction.lua")
        ));

TEST(Base64, Base64_testOneZero)
{
    std::vector<std::uint8_t> input;
    input.emplace_back(0);
    std::vector<std::uint8_t> output;
    dagbase::base64encode((const std::uint8_t*)input.data(), input.size(), &output);
    ASSERT_EQ(4U, output.size());
    EXPECT_EQ('A', output[0]);
    EXPECT_EQ('A', output[1]);
    EXPECT_EQ('=', output[2]);
    EXPECT_EQ('=', output[3]);
    std::vector<std::uint8_t> actualOutput;
    dagbase::base64decode(output.data(), output.size(), &actualOutput);
    ASSERT_EQ(1U, actualOutput.size());
    EXPECT_EQ(0, actualOutput[0]);
}

TEST(Base64, Base64_testTwoZero)
{
    std::vector<std::uint8_t> input;
    input.emplace_back(0);
    input.emplace_back(0);
    std::vector<std::uint8_t> output;
    dagbase::base64encode((const std::uint8_t*)input.data(), input.size(), &output);
    ASSERT_EQ(4U, output.size());
    EXPECT_EQ('A', output[0]);
    EXPECT_EQ('A', output[1]);
    EXPECT_EQ('A', output[2]);
    EXPECT_EQ('=', output[3]);
    std::vector<std::uint8_t> actualOutput;
    dagbase::base64decode(output.data(), output.size(), &actualOutput);
    ASSERT_EQ(2U, actualOutput.size());
    EXPECT_EQ(0, actualOutput[0]);
    EXPECT_EQ(0, actualOutput[1]);
}