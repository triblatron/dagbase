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
        std::make_tuple("110%", 110.0, dagbase::Unit::PERCENT),
        std::make_tuple("8rad", std::fmod(8.0,2.0*M_PI), dagbase::Unit::RADIAN),
        std::make_tuple("20C", 20.0, dagbase::Unit::CELSIUS)
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
        std::make_tuple("0 - 110%", 0.0, 110.0, dagbase::Unit::PERCENT)
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
        std::make_tuple(1, dagbase::Unit::KILOMETRE, dagbase::Unit::METRE, 1000.0, dagbase::Unit::Unit::CONV_OK),
        std::make_tuple(20.0, dagbase::Unit::CELSIUS, dagbase::Unit::KELVIN, 20.0+273.15, dagbase::Unit::Unit::CONV_OK),
        std::make_tuple(100.0, dagbase::Unit::CELSIUS, dagbase::Unit::FAHRENHEIT, 212.0, dagbase::Unit::Unit::CONV_OK),
        std::make_tuple(-273.15, dagbase::Unit::CELSIUS, dagbase::Unit::KELVIN, 0.0, dagbase::Unit::Unit::CONV_OK)
        ));

class Dimension_testCompareEqual : public ::testing::TestWithParam<std::tuple<const char*, const char*, bool>>
{

};

TEST_P(Dimension_testCompareEqual, testExpectedValue)
{
    dagbase::Dimension op1{std::get<0>(GetParam())};
    dagbase::Dimension op2{std::get<1>(GetParam())};
    auto equal = std::get<2>(GetParam());
    EXPECT_EQ(equal, op1 == op2) << "Expected " << op1.symbol << " to be equal to " << op2.symbol;
}

INSTANTIATE_TEST_SUITE_P(Dimension, Dimension_testCompareEqual, ::testing::Values(
    std::make_tuple("M", "M", true),
    std::make_tuple("T^2", "T^2", true),
    std::make_tuple("MLT^-2", "LMT^-2", true),
    std::make_tuple("M", "L", false),
    std::make_tuple("ML^2T^-2", "MLT^-2", false)
    ));

class Dimension_testConvertToPolynomial : public ::testing::TestWithParam<std::tuple<const char*, std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS>>>
{

};

TEST_P(Dimension_testConvertToPolynomial, testExpectedValue)
{
    auto dim = dagbase::Dimension{std::get<0>(GetParam())};
    auto poly = std::get<1>(GetParam());
    std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS> actual{};
    dim.convertToPolynomial(&actual[0]);
    for (int i=0; i<dagbase::NUM_BASE_DIMENSIONS; ++i)
    {
        EXPECT_EQ(poly[i], actual[i]) << "Expected element " << i << " to be equal";
    }
}

INSTANTIATE_TEST_SUITE_P(Dimension, Dimension_testConvertToPolynomial, ::testing::Values(
    std::make_tuple("M", std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS>{0, 1, 0, 0, 0, 0, 0}),
    std::make_tuple("T^1", std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS>{0, 0, 1, 0, 0, 0, 0}),
    std::make_tuple("T^-1", std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS>{0, 0, -1, 0, 0, 0, 0}),
    std::make_tuple("LT^-1", std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS>{1, 0, -1, 0, 0, 0, 0}),
    std::make_tuple("MLT^-2", std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS>{1, 1, -2, 0, 0, 0, 0}),
    std::make_tuple("T^-2ML", std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS>{1, 1, -2, 0, 0, 0, 0}),
    std::make_tuple("T^-2ML^1", std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS>{1, 1, -2, 0, 0, 0, 0}),
    std::make_tuple("T^0", std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS>{0, 0, 0, 0, 0, 0, 0}),
    std::make_tuple("T^+1", std::array<std::int32_t, dagbase::NUM_BASE_DIMENSIONS>{0, 0, 1, 0, 0, 0, 0})
    ));
