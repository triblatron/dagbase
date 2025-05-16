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
