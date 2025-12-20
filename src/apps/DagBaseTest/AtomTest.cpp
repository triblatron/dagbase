//
// Created by Tony Horrobin on 15/04/2025.
//

#include "config/config.h"

#include "core/Atom.h"

#include <gtest/gtest.h>
#include <string>

TEST(Atom, Atom_afterInterningTwoAtomsForTheSameStringCompareEqual)
{
    std::string s1 = "Hello World";
    std::string s2 = "Hello World";

    auto & a1 = dagbase::Atom::intern(s1);
    auto & a2 = dagbase::Atom::intern(s2);

    EXPECT_EQ(a1, a2);
    dagbase::Atom::clear();
}

class Atom_testFind : public ::testing::TestWithParam<std::tuple<const char*, char, std::size_t>>
{

};

TEST_P(Atom_testFind, testExpectedResult)
{
    auto haystack = std::get<0>(GetParam());
    auto needle = std::get<1>(GetParam());
    auto result = std::get<2>(GetParam());
    dagbase::Atom sut = dagbase::Atom::intern(haystack);

    EXPECT_EQ(result, sut.find(needle));
}

INSTANTIATE_TEST_SUITE_P(Atom, Atom_testFind, ::testing::Values(
    std::make_tuple("wibble", 'z', dagbase::Atom::npos),
    std::make_tuple("wibble", 'w', 0U),
    std::make_tuple("wibble", 'b', 2U),
    std::make_tuple("wibble", 'e', 5U)
    ));

class Atom_testSubstr : public ::testing::TestWithParam<std::tuple<const char*, std::size_t, std::size_t, const char*>>
{

};

TEST_P(Atom_testSubstr, testExpectedResult)
{
    auto str = std::get<0>(GetParam());
    auto pos = std::get<1>(GetParam());
    auto len = std::get<2>(GetParam());
    auto result = dagbase::Atom::intern(std::get<3>(GetParam()));

    auto sut = dagbase::Atom::intern(str);
    EXPECT_EQ(result, sut.substr(pos, len));
}

INSTANTIATE_TEST_SUITE_P(Atom, Atom_testSubstr, ::testing::Values(
    std::make_tuple("wibbletyspoo", 0U, dagbase::Atom::npos, "wibbletyspoo"),
    std::make_tuple("wibbletyspoo", 0U, 6, "wibble"),
    std::make_tuple("wibbletyspoo", 8U, dagbase::Atom::npos, "spoo"),
    std::make_tuple("wibbletyspoo", 8U, 4, "spoo")
    ));