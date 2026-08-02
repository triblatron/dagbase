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

struct TestNode
{
    TestNode* parent{nullptr};
    std::uint32_t i{0};
    std::string s{};
    using ChildArray = std::vector<TestNode*>;
    ChildArray children{};
    bool b{false};
    dagbase::ConfigurationElement::ValueType value{};
    TestNode() = default;

    TestNode(dagbase::InputStream& str, dagbase::Lua& lua)
    {
        str.addObj(this);
        std::string className;
        str.readHeader(&className);
        std::string fieldName;
        str.readField(&fieldName);
        dagbase::Stream::ObjId id{0};
        parent = str.readRef<TestNode>(&id, lua);
        str.readField(&fieldName);
        str.readUInt32(&i);
        str.readField(&fieldName);
        str.readString(&s, true);
        str.readField(&fieldName);
        str.readBool(&b);
        str.readField(&fieldName);
        str.read(lua, &value);
        std::uint32_t numChildren{0};
        str.readField(&fieldName);
        str.readUInt32(&numChildren);
        children.reserve(numChildren);
        str.readField(&fieldName);
        str.readHeader(&className);
        for (std::uint32_t i = 0; i < numChildren; ++i)
        {
            dagbase::Stream::ObjId childId{0};

            if (TestNode* child=str.readRef<TestNode>(&childId, lua); child)
                children.emplace_back(child);
        }
        str.readFooter();
        str.readFooter();
    }
    void write(dagbase::OutputStream& str) const
    {
        str.writeHeader("TestNode");
        str.writeField("parent");
        if (str.writeRef(parent))
        {
            parent->write(str);
        }
        str.writeField("i");
        str.writeUInt32(i);
        str.writeField("s");
        str.writeString(s, true);
        str.writeField("b");
        str.writeBool(b);
        str.writeField("value");
        str.write(value);
        str.writeField("numChildren");
        str.writeUInt32(children.size());
        str.writeField("children");
        str.writeHeader("ChildrenArray");
        for (auto child : children)
        {
            if (str.writeRef(child))
                child->write(str);
        }
        str.writeFooter();
        str.writeFooter();
    }
};

class OutputStream_testOutput : public ::testing::TestWithParam<std::tuple<std::string_view>>
{

};

TEST(OutputStream, testOutput)
{
    TestNode node1;
    TestNode node2;
    node2.parent = &node1;
    node2.s = "test";
    node2.b = true;
    node1.value = dagbase::Variant(2.0);
    node2.value = dagbase::Variant(1.0);
    dagbase::MemoryBackingStore store;
    dagbase::OutputStream* sut = dagbase::createOutputStream("TextFormat", store, "");
    node2.write(*sut);
    sut->flush();
    store.open(dagbase::BackingStore::MODE_INPUT_BIT, "");
    std::string_view output = "TestNode\n{\n  parent : 1\n  TestNode\n  {\n    parent : 0\n    i : 0\n    s : \"\"\n    b : false\n    value : true\n1\n2\n    numChildren : 0\n    children :     ChildrenArray\n    {\n    }\n  }\n  i : 0\n  s : \"test\"\n  b : true\n  value : true\n1\n1\n  numChildren : 0\n  children :   ChildrenArray\n  {\n  }\n}\n";
    std::string actualOutput;
    actualOutput.resize(store.numBytesAvailable());
    store.get(actualOutput);
    EXPECT_EQ(output, actualOutput);
}

class FormatAgnosticOutputToInput_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, const char*, const char*>>
{

};

TEST_P(FormatAgnosticOutputToInput_testRoundTrip, testRef)
{
    dagbase::Lua lua;
    std::string formatClass = std::get<0>(GetParam());
    std::string storeClass = std::get<1>(GetParam());
    auto filename = std::get<2>(GetParam());
    dagbase::MemoryBackingStore store;
    dagbase::OutputStream* sut = dagbase::createOutputStream(formatClass, store, filename);

    ASSERT_NE(nullptr, sut);
    TestNode node1;
    node1.i = 3141;
    TestNode node2;
    node2.i = 42;
    node2.parent = &node1;
    node1.children.emplace_back(&node2);
    if (sut->writeRef(&node2))
    {
        node2.write(*sut);
    }
    sut->flush();
    auto istr = dagbase::createInputStream(formatClass, store, filename);

    ASSERT_NE(nullptr, istr);
    dagbase::Stream::ObjId id{~0U};
    auto actual = (istr->readRef<TestNode>(&id, lua));
    ASSERT_NE(nullptr, actual);
    ASSERT_NE(nullptr, actual->parent);
    EXPECT_EQ(42, actual->i);
    EXPECT_EQ(3141, actual->parent->i);
}

INSTANTIATE_TEST_SUITE_P(FormatAgnosticInputStream, FormatAgnosticOutputToInput_testRoundTrip, ::testing::Values(
    std::make_tuple("TextFormat", "MemoryStore", "scratch/FormatAgnosticOutputToInput_testRoundTrip0.txt"),
    std::make_tuple("BinaryFormat", "MemoryStore", "scratch/FormatAgnosticOutputToInput_testRoundTrip1.bin"),
    std::make_tuple("TextFormat", "FileStore", "scratch/FormatAgnosticOutputToInput_testRoundTrip0.txt"),
    std::make_tuple("BinaryFormat", "FileStore", "scratch/FormatAgnosticOutputToInput_testRoundTrip1.bin")
    ));

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
