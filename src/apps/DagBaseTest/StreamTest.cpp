//
// Created by Tony Horrobin on 02/03/2025.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/Class.h"
#include "io/MemoryBackingStore.h"
#include "io/OutputStream.h"
#include "io/Stream.h"
#include "io/BinaryOutputStream.h"
#include "io/TextOutputStream.h"
#include "io/BinaryInputStream.h"
#include "io/StreamFactory.h"
#include "io/TextInputStream.h"

class OutputStream_testWriteVariant : public ::testing::TestWithParam<std::tuple<const char*, const char*, const char*, dagbase::Variant>>
{

};

TEST_P(OutputStream_testWriteVariant, testExpectedValue)
{
    std::string formatClass = std::get<0>(GetParam());
    std::string storeClass = std::get<1>(GetParam());
    auto filename = std::get<2>(GetParam());
    dagbase::BackingStore* store = dagbase::createBackingStore(storeClass);
    dagbase::OutputStream* sut = dagbase::createOutputStream(formatClass, *store, filename);
    ASSERT_NE(nullptr, sut);
    auto value = std::get<3>(GetParam());
    dagbase::Lua lua;
    sut->write(value);
    sut->flush();
    dagbase::InputStream *istr = dagbase::createInputStream(formatClass, *store, filename);
    ASSERT_NE(nullptr, istr);
    dagbase::Variant actualValue{std::numeric_limits<std::uint32_t>::max()};
    istr->read(lua, &actualValue);
    EXPECT_EQ(value, actualValue);
    delete istr;
    delete sut;
}

INSTANTIATE_TEST_SUITE_P(OutputStream, OutputStream_testWriteVariant, ::testing::Values(
        std::make_tuple("TextFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant0.txt", dagbase::Variant()),
        std::make_tuple("BinaryFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant1.bin", dagbase::Variant()),
        std::make_tuple("TextFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant2.txt", std::uint32_t{1}),
        std::make_tuple("BinaryFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant3.bin", std::uint32_t{1}),
        std::make_tuple("TextFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant4.txt", std::int64_t{-1}),
        std::make_tuple("BinaryFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant5.bin", std::int64_t{-1}),
        std::make_tuple("TextFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant6.txt", double{1.5}),
        std::make_tuple("BinaryFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant7.bin", double{1.5}),
        std::make_tuple("TextFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant8.txt", true),
        std::make_tuple("BinaryFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant9.bin", true),
        std::make_tuple("TextFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant10.txt", "test"),
        std::make_tuple("BinaryFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant11.bin", "test"),
        std::make_tuple("TextFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant12.txt", dagbase::Vec2{1.0f, 2.0f}),
        std::make_tuple("BinaryFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant13.bin", dagbase::Vec2{1.0f, 2.0f}),
        std::make_tuple("TextFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant14.txt", dagbase::Colour{1.0f, 0.0f, 1.0f}),
        std::make_tuple("BinaryFormat", "MemoryBackingStore", "scratch/OutputStream_testWriteVariant15.bin", dagbase::Colour{1.0f, 0.0f, 1.0f}),
        std::make_tuple("TextFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant16.txt", dagbase::Variant()),
        std::make_tuple("BinaryFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant17.bin", dagbase::Variant()),
        std::make_tuple("TextFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant18.txt", std::uint32_t{1}),
        std::make_tuple("BinaryFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant19.bin", std::uint32_t{1}),
        std::make_tuple("TextFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant20.txt", std::int64_t{-1}),
        std::make_tuple("BinaryFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant21.bin", std::int64_t{-1}),
        std::make_tuple("TextFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant22.txt", double{1.5}),
        std::make_tuple("BinaryFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant23.bin", double{1.5}),
        std::make_tuple("TextFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant24.txt", true),
        std::make_tuple("BinaryFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant25.bin", true),
        std::make_tuple("TextFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant26.txt", "test"),
        std::make_tuple("BinaryFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant27.bin", "test"),
        std::make_tuple("TextFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant28.txt", dagbase::Vec2{1.0f, 2.0f}),
        std::make_tuple("BinaryFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant29.bin", dagbase::Vec2{1.0f, 2.0f}),
        std::make_tuple("TextFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant30.txt", dagbase::Colour{1.0f, 0.0f, 1.0f}),
        std::make_tuple("BinaryFormat", "FileBackingStore", "scratch/OutputStream_testWriteVariant31.bin", dagbase::Colour{1.0f, 0.0f, 1.0f})
        ));

class OutputStream_testSerialisePrimitive : public ::testing::TestWithParam<std::tuple<const char*, const char*, const char*>>
{

};

TEST_P(OutputStream_testSerialisePrimitive, testExpectedValue)
{
    std::string formatClass = std::get<0>(GetParam());
    std::string storeClass = std::get<1>(GetParam());
    auto filename = std::get<2>(GetParam());
    dagbase::BackingStore* store = dagbase::createBackingStore(storeClass);
    ASSERT_NE(nullptr, store);
    dagbase::OutputStream* sut = dagbase::createOutputStream(formatClass, *store, filename);
    dagbase::InputStream* istr = nullptr;

    ASSERT_NE(nullptr, sut);
    dagbase::Lua lua;
    sut->writeUInt8(1);
    sut->writeInt8(-1);
    sut->writeUInt16(2);
    sut->writeInt16(-2);
    sut->writeUInt32(3);
    sut->writeInt32(-3);
    sut->writeUInt64(4);
    sut->writeInt64(-4);
    sut->writeFloat(1.5f);
    sut->writeDouble(1.5);
    sut->flush();
    istr = createInputStream(formatClass, *store, filename);

    ASSERT_NE(nullptr, istr);
    std::uint8_t actualUint8 = 0;
    istr->readUInt8(&actualUint8);
    EXPECT_EQ(actualUint8, 1);
    std::int8_t actualInt8 = 0;
    istr->readInt8(&actualInt8);
    EXPECT_EQ(actualInt8, -1);
    std::uint16_t actualUint16 = 0;
    istr->readUInt16(&actualUint16);
    EXPECT_EQ(actualUint16, 2);
    std::int16_t actualInt16 = 0;
    istr->readInt16(&actualInt16);
    EXPECT_EQ(actualInt16, -2);
    std::uint32_t actualUint32 = 0;
    istr->readUInt32(&actualUint32);
    EXPECT_EQ(actualUint32, 3);
    std::int32_t actualInt32 = 0;
    istr->readInt32(&actualInt32);
    EXPECT_EQ(actualInt32, -3);
    std::uint64_t actualUint64 = 0;
    istr->readUInt64(&actualUint64);
    EXPECT_EQ(actualUint64, 4);
    std::int64_t actualInt64 = 0;
    istr->readInt64(&actualInt64);
    EXPECT_EQ(actualInt64, -4);
}

INSTANTIATE_TEST_SUITE_P(OutputStream, OutputStream_testSerialisePrimitive, ::testing::Values(
    std::make_tuple("TextFormat", "MemoryBackingStore", ""),
    std::make_tuple("BinaryFormat", "MemoryBackingStore", ""),
    std::make_tuple("TextFormat", "FileBackingStore", "scratch/OutputStream_testSerialisePrimitive2.txt"),
    std::make_tuple("BinaryFormat", "FileBackingStore", "scratch/OutputStream_testSerialisePrimitive3.bin")
    ));

class InputStream_testReadEmptyBuffer : public ::testing::TestWithParam<std::tuple<const char*, const char*, const char*>>
{

};

TEST_P(InputStream_testReadEmptyBuffer, testExpectedValue)
{
    std::string formatClass = std::get<0>(GetParam());
    std::string storeClass = std::get<1>(GetParam());
    auto filename = std::get<2>(GetParam());
    dagbase::BackingStore* store = dagbase::createBackingStore(storeClass);
    ASSERT_NE(nullptr, store);
    dagbase::InputStream* sut = nullptr;
    sut = dagbase::createInputStream(formatClass, *store, filename);
    ASSERT_NE(nullptr, sut);
    std::int32_t actual{0};
    sut->readInt32(&actual);
    EXPECT_EQ(actual, 0);
}

INSTANTIATE_TEST_SUITE_P(InputStream, InputStream_testReadEmptyBuffer, ::testing::Values(
    std::make_tuple("TextFormat", "MemoryBackingStore", ""),
    std::make_tuple("BinaryFormat", "MemoryBackingStore", ""),
    std::make_tuple("TextFormat", "FileBackingStore", "scratch/InputStream_testReadEmptyBuffer2.txt"),
    std::make_tuple("BinaryFormat", "FileBackingStore", "scratch/InputStream_testReadEmptyBuffer3.txt")
    ));
