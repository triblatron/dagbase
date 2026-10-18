//
// Created by Tony Horrobin on 18/10/2026.
//

#include "config/config.h"

#include "core/Schema.h"

#include <gtest/gtest.h>

class SchemaVersion_testToString : public ::testing::TestWithParam<std::tuple<dagbase::SchemaVersion, const char*>>
{

};

TEST_P(SchemaVersion_testToString, testExpectedValue)
{
    auto version = std::get<0>(GetParam());
    auto str = std::get<1>(GetParam());

    std::string actual = version.toString();
    EXPECT_EQ(str, actual);
}

INSTANTIATE_TEST_SUITE_P(SchemaVersion, SchemaVersion_testToString, ::testing::Values(
    std::make_tuple(dagbase::SchemaVersion{0,1,0,dagbase::VERSION_NONE}, "0.1.0"),
    std::make_tuple(dagbase::SchemaVersion{0,1,1,dagbase::VERSION_NONE}, "0.1.1"),
    std::make_tuple(dagbase::SchemaVersion{1,1,1,dagbase::VERSION_NONE}, "1.1.1"),
    std::make_tuple(dagbase::SchemaVersion{1,0,0,dagbase::VERSION_ALPHA}, "1.0.0-alpha"),
    std::make_tuple(dagbase::SchemaVersion{1,0,0,dagbase::SchemaVersion::makeFlags(dagbase::VERSION_ALPHA, 1)}, "1.0.0-alpha.1"),
    std::make_tuple(dagbase::SchemaVersion{1,0,0,dagbase::VERSION_BETA}, "1.0.0-beta"),
    std::make_tuple(dagbase::SchemaVersion{1,0,0,dagbase::SchemaVersion::makeFlags(dagbase::VERSION_BETA, 1)}, "1.0.0-beta.1"),
    std::make_tuple(dagbase::SchemaVersion{1,0,0,dagbase::SchemaVersion::makeFlags(dagbase::VERSION_RC, 1)}, "1.0.0-rc.1"),
    std::make_tuple(dagbase::SchemaVersion{1,0,0,dagbase::SchemaVersion::makeFlags(dagbase::VERSION_RC, 15)}, "1.0.0-rc.15"),
    std::make_tuple(dagbase::SchemaVersion{1,0,0,dagbase::SchemaVersion::makeFlags(dagbase::VERSION_RC, 16)}, "1.0.0-rc")
    ));