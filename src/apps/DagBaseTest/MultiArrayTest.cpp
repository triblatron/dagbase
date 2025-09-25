//
// Created by Tony Horrobin on 25/09/2025.
//

#include "util/MultiArray.h"

#include <gtest/gtest.h>

class MultiArray_testEmplaceBack : public ::testing::TestWithParam<std::tuple<int, double, std::string>>
{

};

TEST_P(MultiArray_testEmplaceBack, testExpectedValues)
{
    dagbase::MultiArray<int, double, std::string> sut;
    auto i = std::get<0>(GetParam());
    auto d = std::get<1>(GetParam());
    auto s = std::get<2>(GetParam());
    sut.reserve(1);
    sut.emplace_back(i,d,s);
    EXPECT_EQ(i, sut.array<0>()[0]);
    EXPECT_EQ(d, sut.array<1>()[0]);
    EXPECT_EQ(s, sut.array<2>()[0]);
}

INSTANTIATE_TEST_SUITE_P(MultiArray, MultiArray_testEmplaceBack, ::testing::Values(
        std::make_tuple(1, 2.5, "test")
        ));