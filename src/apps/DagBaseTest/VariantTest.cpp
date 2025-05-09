//
// Created by Tony Horrobin on 28/04/2025.
//

#include "core/Variant.h"

#include <gtest/gtest.h>

class Variant_testToString : public ::testing::TestWithParam<std::tuple<dagbase::Variant, std::string>>
{

};

TEST_P(Variant_testToString, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto str = std::get<1>(GetParam());
    EXPECT_EQ(str, sut.toString());
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testToString, ::testing::Values(
        std::make_tuple("test", "test"),
        std::make_tuple(std::int64_t(1), "1"),
        std::make_tuple(1.5, "1.500000"),
        std::make_tuple(true, "1"),
        std::make_tuple(dagbase::Variant(dagbase::Colour{0.5f, 0.8f, 0.2f, 1.0f}), "Colour { 0.500000, 0.800000, 0.200000, 1.000000 }"),
        std::make_tuple(dagbase::Variant(dagbase::Vec2{ 1.5f, 10.0f }), "Vec2 { 1.500000, 10.000000 }")
        ));

class Variant_testAsInteger : public ::testing::TestWithParam<std::tuple<dagbase::Variant, std::int64_t>>
{

};

TEST_P(Variant_testAsInteger, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asInteger(0));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsInteger, ::testing::Values(
        std::make_tuple(std::int64_t(1), 1),
        std::make_tuple(1.5, 0),
        std::make_tuple("test", 0),
        std::make_tuple(true, 0),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, 0),
        std::make_tuple(dagbase::Vec2{1.0f,0.0f}, 0)
        ));
class Variant_testAsDouble : public ::testing::TestWithParam<std::tuple<dagbase::Variant, double>>
{

};

TEST_P(Variant_testAsDouble, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asDouble(0.0));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsDouble, ::testing::Values(
        std::make_tuple(std::int64_t(1), 0.0),
        std::make_tuple(1.5, 1.5),
        std::make_tuple("test", 0),
        std::make_tuple(true, 0),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, 0),
        std::make_tuple(dagbase::Vec2{1.0f,0.0f}, 0)
        ));

class Variant_testAsBool : public ::testing::TestWithParam<std::tuple<dagbase::Variant, bool>>
{

};

TEST_P(Variant_testAsBool, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asBool(false));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsBool, ::testing::Values(
        std::make_tuple(std::int64_t(1), false),
        std::make_tuple(1.5, false),
        std::make_tuple("test", false),
        std::make_tuple(true, true),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, false),
        std::make_tuple(dagbase::Vec2{1.0f,0.0f}, false)
        ));


class Variant_testAsString : public ::testing::TestWithParam<std::tuple<dagbase::Variant, std::string>>
{

};

TEST_P(Variant_testAsString, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asString(""));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsString, ::testing::Values(
        std::make_tuple(std::int64_t(1), ""),
        std::make_tuple(1.5, ""),
        std::make_tuple("test", "test"),
        std::make_tuple(true, ""),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, ""),
        std::make_tuple(dagbase::Vec2{1.0f,0.0f}, "")
        ));
class Variant_testAsColour : public ::testing::TestWithParam<std::tuple<dagbase::Variant, dagbase::Colour>>
{

};

TEST_P(Variant_testAsColour, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asColour(dagbase::Colour{0.0f,0.0f,0.0f,0.0f}));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsColour, ::testing::Values(
        std::make_tuple(std::int64_t(1), dagbase::Colour{0.0f,0.0f,0.0f,0.0f}),
        std::make_tuple(1.5, dagbase::Colour{0.0f,0.0f,0.0f,0.0f}),
        std::make_tuple("test", dagbase::Colour{0.0f,0.0f,0.0f,0.0f}),
        std::make_tuple(true, dagbase::Colour{0.0f,0.0f,0.0f,0.0f}),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, dagbase::Colour{1.0f,1.0f,1.0f,1.0f}),
        std::make_tuple(dagbase::Vec2{1.0f,0.0f}, dagbase::Colour{0.0f,0.0f,0.0f,0.0f})
        ));

class Variant_testAsVec2 : public ::testing::TestWithParam<std::tuple<dagbase::Variant, dagbase::Vec2>>
{

};

TEST_P(Variant_testAsVec2, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asVec2(dagbase::Vec2{0.0f,0.0f}));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsVec2, ::testing::Values(
        std::make_tuple(std::int64_t(1), dagbase::Vec2{0.0f,0.0f}),
        std::make_tuple(1.5, dagbase::Vec2{0.0f,0.0f}),
        std::make_tuple("test", dagbase::Vec2{0.0f,0.0f}),
        std::make_tuple(true, dagbase::Vec2{0.0f,0.0f}),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, dagbase::Vec2{0.0f,0.0f}),
        std::make_tuple(dagbase::Vec2{1.0f,1.0f}, dagbase::Vec2{1.0f,1.0f})
        ));

