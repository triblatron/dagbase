//
// Created by Tony Horrobin on 14/05/2025.
//

#include "config/config.h"

#include "core/Unit.h"

#include <gtest/gtest.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

class Unit_testParse : public ::testing::TestWithParam<std::tuple<const char*, double, dagbase::Unit>>
{

};

TEST_P(Unit_testParse, testExpectedUnit)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    auto unit = std::get<2>(GetParam());

    double actualValue = std::numeric_limits<double>::max();
    dagbase::Unit actualUnit;
    dagbase::Unit::parseQuantity(str, &actualValue, &actualUnit);
    EXPECT_EQ(value, actualValue);
    EXPECT_EQ(unit, actualUnit);
}

INSTANTIATE_TEST_SUITE_P(Unit, Unit_testParse, ::testing::Values(
        std::make_tuple("", 0.0, dagbase::Unit::NONE),
        std::make_tuple("1", 1.0, dagbase::Unit::NONE),
        std::make_tuple("1s", 1.0, dagbase::Unit::SECOND),
        std::make_tuple("1m", 1.0, dagbase::Unit::METRE),
        std::make_tuple("1ms^-1", 1.0, dagbase::Unit::METREPERSECOND),
        std::make_tuple("1mph", 1.0, dagbase::Unit::MILEPERHOUR),
        std::make_tuple("1kph", 1.0, dagbase::Unit::KILOMETREPERHOUR),
        std::make_tuple("2.0 kph", 2.0, dagbase::Unit::KILOMETREPERHOUR),
        std::make_tuple("100%", 100.0, dagbase::Unit::PERCENT),
        std::make_tuple("110%", 100.0, dagbase::Unit::PERCENT),
        std::make_tuple("8rad", std::fmod(8.0,2.0*M_PI), dagbase::Unit::RADIAN)
        ));

class Unit_testParseRange : public ::testing::TestWithParam<std::tuple<const char*, double, double, dagbase::Unit>>
{

};

TEST_P(Unit_testParseRange, testExpectedRange)
{
    auto str = std::get<0>(GetParam());
    auto minValue = std::get<1>(GetParam());
    auto maxValue = std::get<2>(GetParam());
    auto unit = std::get<3>(GetParam());

    double actualMinValue=0.0;
    double actualMaxValue=0.0;
    dagbase::Unit actualUnit;
    dagbase::Unit::parseRange(str, &actualMinValue, &actualMaxValue, &actualUnit);
    EXPECT_EQ(minValue, actualMinValue);
    EXPECT_EQ(maxValue, actualMaxValue);
    EXPECT_EQ(unit, actualUnit);
}

INSTANTIATE_TEST_SUITE_P(Unit, Unit_testParseRange, ::testing::Values(
        std::make_tuple("", -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), dagbase::Unit::NONE),
        std::make_tuple("%", 0.0, 100.0, dagbase::Unit::PERCENT),
        std::make_tuple("0-100%", 0.0, 100.0, dagbase::Unit::PERCENT),
        std::make_tuple("0-%", 0.0, 100.0, dagbase::Unit::PERCENT),
        std::make_tuple("0+%", 0.0, 100.0, dagbase::Unit::PERCENT),
        std::make_tuple("50N", 50.0, 50.0, dagbase::Unit::NEWTON),
        std::make_tuple("-5.0-5.0ms^-2", -5.0, 5.0, dagbase::Unit::METREPERSECONDSQUARED),
        std::make_tuple("-5.0--2.5ms^-2", -5.0, -2.5, dagbase::Unit::METREPERSECONDSQUARED),
        std::make_tuple("-5.0 - -2.5 ms^-2", -5.0, -2.5, dagbase::Unit::METREPERSECONDSQUARED),
        std::make_tuple("18+", 18.0, std::numeric_limits<double>::infinity(), dagbase::Unit::NONE),
        std::make_tuple("18+m", 18.0, std::numeric_limits<double>::infinity(), dagbase::Unit::METRE),
        std::make_tuple("18 + m", 18.0, std::numeric_limits<double>::infinity(), dagbase::Unit::METRE),
        std::make_tuple("0-m/s", 0.0, std::numeric_limits<double>::infinity(), dagbase::Unit::METREPERSECOND),
        std::make_tuple("0 - m/s", 0.0, std::numeric_limits<double>::infinity(), dagbase::Unit::METREPERSECOND),
        std::make_tuple("0 - 70%", 0.0, 70.0, dagbase::Unit::PERCENT),
        std::make_tuple("0 - 110%", 0.0, 100.0, dagbase::Unit::PERCENT)
        ));

class Unit_testConvert : public ::testing::TestWithParam<std::tuple<double, dagbase::Unit, dagbase::Unit, double, dagbase::Unit::ConversionResult>>
{

};

TEST_P(Unit_testConvert, testExpectedValue)
{
    auto sourceValue = std::get<0>(GetParam());
    auto sourceUnit = std::get<1>(GetParam());
    auto destUnit = std::get<2>(GetParam());
    auto destValue = std::get<3>(GetParam());
    auto result = std::get<4>(GetParam());
    double actualValue=0.0;
    auto actualResult = dagbase::Unit::convert(sourceValue, sourceUnit, destUnit, &actualValue);
    EXPECT_EQ(result, actualResult);
    EXPECT_NEAR(destValue, actualValue, 1e-3);
}

INSTANTIATE_TEST_SUITE_P(Unit, Unit_testConvert, ::testing::Values(
        std::make_tuple(30.0, dagbase::Unit::MILEPERHOUR, dagbase::Unit::METREPERSECOND, 13.411, dagbase::Unit::Unit::CONV_OK),
        std::make_tuple(30.0, dagbase::Unit::MILEPERHOUR, dagbase::Unit::KILOMETREPERHOUR, 48.28, dagbase::Unit::Unit::CONV_OK),
        std::make_tuple(48.28, dagbase::Unit::KILOMETREPERHOUR, dagbase::Unit::METREPERSECOND, 13.411, dagbase::Unit::Unit::CONV_OK),
        std::make_tuple(48.28, dagbase::Unit::KILOMETREPERHOUR, dagbase::Unit::METRE, 0.0, dagbase::Unit::Unit::CONV_INCOMPATIBLE_DIMS),
        std::make_tuple(1, dagbase::Unit::KILOMETRE, dagbase::Unit::METRE, 1000.0, dagbase::Unit::Unit::CONV_OK)
        ));
