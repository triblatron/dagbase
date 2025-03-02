//
// Created by Tony Horrobin on 02/03/2025.
//

#include "io/StreamFormat.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "io/BinaryFormat.h"
#include "io/MemoryBackingStore.h"
#include "io/TextFormat.h"

class TextFormat_testUInt32 : public ::testing::TestWithParam<std::tuple<const char*, std::uint32_t>>
{

};

TEST_P(TextFormat_testUInt32, testRoundTrip)
{
    auto fieldName = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    dagbase::TextFormat sut;
    dagbase::MemoryBackingStore backingStore(dagbase::BackingStore::MODE_OUTPUT_BIT);
    sut.writeUInt32(backingStore, "test", value);
    backingStore.open(dagbase::BackingStore::MODE_INPUT_BIT);
    std::uint32_t actualValue{0};
    std::string actualFieldName;
    sut.readUInt32(backingStore, &actualFieldName, &actualValue);
    EXPECT_EQ(fieldName, actualFieldName);
    EXPECT_EQ(value, actualValue);
}

INSTANTIATE_TEST_SUITE_P(TextFormat, TextFormat_testUInt32, ::testing::Values(
    std::make_tuple("test", 42)
    ));
