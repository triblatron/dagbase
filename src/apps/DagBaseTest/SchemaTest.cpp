//
// Created by Tony Horrobin on 18/10/2026.
//

#include "config/config.h"

#include "core/Schema.h"
#include "io/StreamFactory.h"
#include "io/MemoryBackingStore.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"

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
    std::make_tuple(dagbase::SchemaVersion{1,0,0,dagbase::SchemaVersion::makeFlags(dagbase::VERSION_RC, 16)}, "1.0.0-rc.16"),
    std::make_tuple(dagbase::SchemaVersion{1,0,0,dagbase::SchemaVersion::makeFlags(dagbase::VERSION_RC, 63)}, "1.0.0-rc.63"),
    std::make_tuple(dagbase::SchemaVersion{1,0,0,dagbase::SchemaVersion::makeFlags(dagbase::VERSION_RC, 64)}, "1.0.0-rc")
    ));

class SchemaHeader_testStreaming : public ::testing::TestWithParam<std::tuple<dagbase::SchemaHeader, const char*>>
{

};

TEST_P(SchemaHeader_testStreaming, testRoundTrip)
{
    auto header = std::get<0>(GetParam());
    auto format = std::get<1>(GetParam());
    dagbase::BackingStore* store = dagbase::createBackingStore("MemoryBackingStore");
    ASSERT_NE(nullptr, store);
    auto ostr = dagbase::createOutputStream(format,*store,"");
    ASSERT_NE(nullptr, ostr);
    header.writeToStream(*ostr);
    ostr->flush();
    delete ostr;
    auto istr = dagbase::createInputStream(format, *store, "");
    dagbase::SchemaHeader actual;
    actual.readFromStream(*istr);
    EXPECT_EQ(header, actual);
    delete istr;
    delete store;
}

INSTANTIATE_TEST_SUITE_P(SchemaHeader, SchemaHeader_testStreaming, ::testing::Values(
    std::make_tuple(dagbase::SchemaHeader{0xdeadbeef, {1, 0, 0, dagbase::SchemaVersion::makeFlags(dagbase::VERSION_NONE, 0)}}, "TextFormat"),
    std::make_tuple(dagbase::SchemaHeader{0xdeadbeef, {1, 0, 0, dagbase::SchemaVersion::makeFlags(dagbase::VERSION_NONE, 0)}}, "BinaryFormat"),
    std::make_tuple(dagbase::SchemaHeader{0xdeadbeef, {1, 0, 1, dagbase::SchemaVersion::makeFlags(dagbase::VERSION_RC, 4)}}, "TextFormat"),
    std::make_tuple(dagbase::SchemaHeader{0xdeadbeef, {1, 0, 1, dagbase::SchemaVersion::makeFlags(dagbase::VERSION_RC, 4)}}, "BinaryFormat")
    ));
