//
// Created by Tony Horrobin on 14/05/2025.
//

#include "core/Unit.h"

#include <gtest/gtest.h>

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
        std::make_tuple("", 0.0, dagbase::NONE),
        std::make_tuple("1", 1.0, dagbase::NONE),
        std::make_tuple("1s", 1.0, dagbase::SECOND),
        std::make_tuple("1m", 1.0, dagbase::METRE),
        std::make_tuple("1ms^-1", 1.0, dagbase::METREPERSECOND),
        std::make_tuple("1mph", 1.0, dagbase::MILEPERHOUR),
        std::make_tuple("1kph", 1.0, dagbase::KILOMETERPERHOUR),
        std::make_tuple("2.0 kph", 2.0, dagbase::KILOMETERPERHOUR)
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
        std::make_tuple(30.0, dagbase::MILEPERHOUR, dagbase::METREPERSECOND, 13.411, dagbase::Unit::CONV_OK),
        std::make_tuple(30.0, dagbase::MILEPERHOUR, dagbase::KILOMETERPERHOUR, 48.28, dagbase::Unit::CONV_OK),
        std::make_tuple(48.28, dagbase::KILOMETERPERHOUR, dagbase::METREPERSECOND, 13.411, dagbase::Unit::CONV_OK),
        std::make_tuple(48.28, dagbase::KILOMETERPERHOUR, dagbase::METRE, 0.0, dagbase::Unit::CONV_INCOMPATIBLE_DIMS)
        ));
