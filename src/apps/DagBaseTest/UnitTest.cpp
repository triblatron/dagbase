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
        std::make_tuple("1s", 1.0, dagbase::SECOND),
        std::make_tuple("1m", 1.0, dagbase::METRE)
        ));
