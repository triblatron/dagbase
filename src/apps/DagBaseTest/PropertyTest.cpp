//
// Created by Tony Horrobin on 27/04/2025.
//

#include "core/MetaProperty.h"
#include "core/ConfigurationElement.h"
#include "core/LuaInterface.h"
#include "test/TestUtils.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MetaProperty_testConfigure : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::ConfigurationElement::ValueType, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(MetaProperty_testConfigure, testExpectedValues)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::MetaProperty sut;
    sut.configure(*config);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actualValue = sut.find(path);
    assertComparison(value, actualValue, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(MetaProperty, MetaProperty_testConfigure, ::testing::Values(
        std::make_tuple("data/tests/MetaProperty/NoSemantic.lua", "dataType", "DATATYPE_STRING", 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/MetaProperty/NoSemantic.lua", "format", "FORMAT_NONE", 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

class MetaPropertyDataType_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::MetaProperty::DataType>>
{

};

TEST_P(MetaPropertyDataType_testRoundTrip, testExpectedValue)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::MetaProperty::dataTypeToString(value));
    EXPECT_EQ(value, dagbase::MetaProperty::parseDataType(str));
}

INSTANTIATE_TEST_SUITE_P(MetaPropertyDataType, MetaPropertyDataType_testRoundTrip, ::testing::Values(
        std::make_tuple("DATATYPE_UNKNOWN", dagbase::MetaProperty::DATATYPE_UNKNOWN),
        std::make_tuple("DATATYPE_STRING", dagbase::MetaProperty::DATATYPE_STRING),
        std::make_tuple("DATATYPE_INT", dagbase::MetaProperty::DATATYPE_INT),
        std::make_tuple("DATATYPE_BOOL", dagbase::MetaProperty::DATATYPE_BOOL),
        std::make_tuple("DATATYPE_DOUBLE", dagbase::MetaProperty::DATATYPE_DOUBLE)
        ));

class MetaPropertyFormat_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::MetaProperty::Format>>
{

};

TEST_P(MetaPropertyFormat_testRoundTrip, testExpectedValue)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::MetaProperty::formatToString(value));
    EXPECT_EQ(value, dagbase::MetaProperty::parseFormat(str));
}

INSTANTIATE_TEST_SUITE_P(MetaPropertyFormat, MetaPropertyFormat_testRoundTrip, ::testing::Values(
        std::make_tuple("FORMAT_UNKNOWN", dagbase::MetaProperty::FORMAT_UNKNOWN),
        std::make_tuple("FORMAT_NONE", dagbase::MetaProperty::FORMAT_NONE),
        std::make_tuple("FORMAT_COLOUR", dagbase::MetaProperty::FORMAT_COLOUR)
        ));
