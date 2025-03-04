//
// Created by Tony Horrobin on 02/03/2025.
//

#include "io/StreamFormat.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "io/BinaryFormat.h"
#include "io/MemoryBackingStore.h"
#include "io/TextFormat.h"

class StreamFormat_testUInt32 : public ::testing::TestWithParam<std::tuple<std::string_view, std::uint32_t>>
{

};

TEST_P(StreamFormat_testUInt32, testRoundTrip)
{
    auto className = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    dagbase::StreamFormat* sut = nullptr;
    if (className == "TextFormat")
        sut = new dagbase::TextFormat();
    else if (className == "BinaryFormat")
        sut = new dagbase::BinaryFormat();
    ASSERT_NE(nullptr, sut);
    dagbase::MemoryBackingStore backingStore(dagbase::BackingStore::MODE_OUTPUT_BIT);
    sut->writeUInt32(backingStore, value);
    backingStore.open(dagbase::BackingStore::MODE_INPUT_BIT);
    std::uint32_t actualValue{0};
    sut->readUInt32(backingStore, &actualValue);
    EXPECT_EQ(value, actualValue);
}

INSTANTIATE_TEST_SUITE_P(StreamFormat, StreamFormat_testUInt32, ::testing::Values(
    std::make_tuple("TextFormat", 42),
    std::make_tuple("BinaryFormat", 42)
    ));
