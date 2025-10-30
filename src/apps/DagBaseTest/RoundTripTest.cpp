//
// Created by Tony Horrobin on 27/09/2025.
//

#include "core/Variant.h"
#include "core/ConfigurationElement.h"

#include <gtest/gtest.h>

class VariantIndex_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::Variant::Index>>
{

};

TEST_P(VariantIndex_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::Variant::indexToString(value));
    EXPECT_EQ(value, dagbase::Variant::parseIndex(str));
}
//TYPE_INTEGER,
//TYPE_DOUBLE,
//TYPE_BOOL,
//TYPE_STRING,
//TYPE_COLOUR,
//TYPE_VEC2,
//TYPE_UINT

INSTANTIATE_TEST_SUITE_P(Variant, VariantIndex_testRoundTrip, ::testing::Values(
        std::make_tuple("TYPE_INTEGER", dagbase::Variant::TYPE_INTEGER),
        std::make_tuple("TYPE_DOUBLE", dagbase::Variant::TYPE_DOUBLE),
        std::make_tuple("TYPE_BOOL", dagbase::Variant::TYPE_BOOL),
        std::make_tuple("TYPE_STRING", dagbase::Variant::TYPE_STRING),
        std::make_tuple("TYPE_COLOUR", dagbase::Variant::TYPE_COLOUR),
        std::make_tuple("TYPE_VEC2", dagbase::Variant::TYPE_VEC2),
        std::make_tuple("TYPE_UINT", dagbase::Variant::TYPE_UINT)
        ));

class ConfigurationElementRelOp_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(ConfigurationElementRelOp_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::ConfigurationElement::relOpToString(value));
    EXPECT_EQ(value, dagbase::ConfigurationElement::parseRelOp(str));
}

INSTANTIATE_TEST_SUITE_P(ConfigurationElement, ConfigurationElementRelOp_testRoundTrip, ::testing::Values(
        std::make_tuple("RELOP_UNKNOWN", dagbase::ConfigurationElement::RELOP_UNKNOWN),
        std::make_tuple("RELOP_EQ", dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("RELOP_NE", dagbase::ConfigurationElement::RELOP_NE),
        std::make_tuple("RELOP_LT", dagbase::ConfigurationElement::RELOP_LT),
        std::make_tuple("RELOP_LE", dagbase::ConfigurationElement::RELOP_LE),
        std::make_tuple("RELOP_GT", dagbase::ConfigurationElement::RELOP_GT),
        std::make_tuple("RELOP_GE", dagbase::ConfigurationElement::RELOP_GE)
        ));