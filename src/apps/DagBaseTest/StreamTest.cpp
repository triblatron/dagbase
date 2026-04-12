//
// Created by Tony Horrobin on 02/03/2025.
//

#include "io/StreamFormat.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/Class.h"
#include "io/BinaryFormat.h"
#include "io/FormatAgnosticOutputStream.h"
#include "io/MemoryBackingStore.h"
#include "io/OutputStream.h"
#include "io/Stream.h"
#include "io/TextFormat.h"
#include "io/FormatAgnosticInputStream.h"
#include "io/BinaryOutputStream.h"
#include "io/TextOutputStream.h"
#include "io/BinaryInputStream.h"
#include "io/TextInputStream.h"
#include "TestObject.h"

class StreamFormat_testUInt32 : public ::testing::TestWithParam<std::tuple<std::string_view, std::uint32_t>>
{

};

TEST_P(StreamFormat_testUInt32, testRoundTrip)
{
    auto className = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    dagbase::StreamFormat* sut = nullptr;
    dagbase::MemoryBackingStore backingStore(dagbase::BackingStore::MODE_OUTPUT_BIT);
    if (className == "TextFormat")
        sut = new dagbase::TextFormat(&backingStore);
    else if (className == "BinaryFormat")
        sut = new dagbase::BinaryFormat(&backingStore);
    ASSERT_NE(nullptr, sut);
    sut->setMode(dagbase::StreamFormat::MODE_OUTPUT);
    sut->writeUInt32(value);
    sut->flush();
    sut->setMode(dagbase::StreamFormat::MODE_INPUT);
    backingStore.open(dagbase::BackingStore::MODE_INPUT_BIT);
    std::uint32_t actualValue{0};
    sut->readUInt32(&actualValue);
    EXPECT_EQ(value, actualValue);
}

INSTANTIATE_TEST_SUITE_P(StreamFormat, StreamFormat_testUInt32, ::testing::Values(
    std::make_tuple("TextFormat", 42),
    std::make_tuple("BinaryFormat", 42)
    ));

class StreamFormat_testDouble : public ::testing::TestWithParam<std::tuple<std::string_view, double>>
{

};

TEST_P(StreamFormat_testDouble, testRoundTrip)
{
    auto className = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    dagbase::StreamFormat* sut = nullptr;
    dagbase::MemoryBackingStore backingStore(dagbase::BackingStore::MODE_OUTPUT_BIT);
    if (className == "TextFormat")
        sut = new dagbase::TextFormat(&backingStore);
    else if (className == "BinaryFormat")
        sut = new dagbase::BinaryFormat(&backingStore);
    ASSERT_NE(nullptr, sut);
    sut->setMode(dagbase::StreamFormat::MODE_OUTPUT);
    sut->writeDouble(value);
    sut->flush();
    sut->setMode(dagbase::StreamFormat::MODE_INPUT);
    backingStore.open(dagbase::BackingStore::MODE_INPUT_BIT);
    double actualValue{0};
    sut->readDouble(&actualValue);
    EXPECT_EQ(value, actualValue);
}

INSTANTIATE_TEST_SUITE_P(StreamFormat, StreamFormat_testDouble, ::testing::Values(
    std::make_tuple("TextFormat", 3.142),
    std::make_tuple("BinaryFormat", 3.142)
    ));

class StreamFormat_testString : public ::testing::TestWithParam<std::tuple<std::string_view, const char*, bool>>
{

};

TEST_P(StreamFormat_testString, testRoundTrip)
{
    auto className = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    auto quoted = std::get<2>(GetParam());
    dagbase::StreamFormat* sut = nullptr;
    dagbase::MemoryBackingStore backingStore(dagbase::BackingStore::MODE_OUTPUT_BIT);
    if (className == "TextFormat")
        sut = new dagbase::TextFormat(&backingStore);
    else if (className == "BinaryFormat")
        sut = new dagbase::BinaryFormat(&backingStore);
    ASSERT_NE(nullptr, sut);
    sut->setMode(dagbase::StreamFormat::MODE_OUTPUT);
    sut->writeString(value, quoted);
    sut->flush();
    sut->setMode(dagbase::StreamFormat::MODE_INPUT);
    backingStore.open(dagbase::BackingStore::MODE_INPUT_BIT);
    std::string actualValue{};
    sut->readString(&actualValue, quoted);
    EXPECT_EQ(value, actualValue);
}

INSTANTIATE_TEST_SUITE_P(StreamFormat, StreamFormat_testString, ::testing::Values(
    std::make_tuple("TextFormat", "test", false),
    std::make_tuple("TextFormat", "test", true),
    std::make_tuple("BinaryFormat", "test", false),
    std::make_tuple("BinaryFormat", "test", true)
    ));

class StreamFormat_testObject : public ::testing::TestWithParam<std::tuple<std::string_view, std::uint32_t>>
{

};

TEST_P(StreamFormat_testObject, testRoundTrip)
{
    auto className = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    dagbase::StreamFormat* sut = nullptr;
    dagbase::MemoryBackingStore backingStore(dagbase::BackingStore::MODE_OUTPUT_BIT);
    if (className == "TextFormat")
        sut = new dagbase::TextFormat(&backingStore);
    else if (className == "BinaryFormat")
        sut = new dagbase::BinaryFormat(&backingStore);
    ASSERT_NE(nullptr, sut);
    sut->setMode(dagbase::StreamFormat::MODE_OUTPUT);
    TestObject obj;
    obj.value = value;
    sut->writeObject(&obj);
    sut->flush();
    backingStore.open(dagbase::BackingStore::MODE_INPUT_BIT);
    sut->setMode(dagbase::StreamFormat::MODE_INPUT);
    TestObject actualObj;
    sut->readObject(&actualObj);
    EXPECT_EQ(value, actualObj.value);
}

INSTANTIATE_TEST_SUITE_P(StreamFormat, StreamFormat_testObject, ::testing::Values(
    std::make_tuple("TextFormat", 42),
    std::make_tuple("BinaryFormat", 42)
    ));

class TextFormat_testOutput : public ::testing::TestWithParam<std::tuple<std::string_view>>
{

};

TEST_P(TextFormat_testOutput, testOutput)
{
    auto output = std::get<0>(GetParam());
    dagbase::MemoryBackingStore backingStore(dagbase::BackingStore::MODE_OUTPUT_BIT);
    dagbase::TextFormat* sut = new dagbase::TextFormat(&backingStore);
    sut->setMode(dagbase::StreamFormat::MODE_OUTPUT);
    TestObject obj;
    obj.value = 42;
    sut->writeObject(&obj);
    sut->flush();
    backingStore.open(dagbase::BackingStore::MODE_INPUT_BIT);
    sut->setMode(dagbase::StreamFormat::MODE_INPUT);
    std::string actualOutput;
    actualOutput.resize(backingStore.numBytesAvailable());
    backingStore.get(actualOutput);
    EXPECT_EQ(output, actualOutput);
}

INSTANTIATE_TEST_SUITE_P(TextFormat, TextFormat_testOutput, ::testing::Values(
    std::make_tuple("TestObject\n{\n  Class\n  {\n    errod : 0\n  }\n  value : 42\n}\n")
    ));

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
    dagbase::MemoryBackingStore store(dagbase::BackingStore::MODE_OUTPUT_BIT);

    dagbase::TextOutputStream sut(&store);
    node2.write(sut);
    sut.flush();

    std::string_view output = "TestNode\n{\n  parent : 1\n  TestNode\n  {\n    parent : 0\n    i : 0\n    s : \"\"\n    b : false\n    value : true\n1\n2\n    numChildren : 0\n    children :     ChildrenArray\n    {\n    }\n  }\n  i : 0\n  s : \"test\"\n  b : true\n  value : true\n1\n1\n  numChildren : 0\n  children :   ChildrenArray\n  {\n  }\n}\n";
    std::string actualOutput;
    actualOutput.resize(store.numBytesAvailable());
    store.get(actualOutput);
    EXPECT_EQ(output, actualOutput);
}

class FormatAgnosticOutputToInput_testRoundTrip : public ::testing::TestWithParam<std::tuple<std::string_view>>
{

};

TEST_P(FormatAgnosticOutputToInput_testRoundTrip, testRef)
{
    dagbase::Lua lua;
    auto formatClass = std::get<0>(GetParam());
    dagbase::OutputStream* sut = nullptr;
    dagbase::MemoryBackingStore store(dagbase::BackingStore::MODE_OUTPUT_BIT);
    if (formatClass == "TextFormat")
    {
        sut = new dagbase::TextOutputStream(&store);
    }
    else if (formatClass == "BinaryFormat")
    {
        sut = new dagbase::BinaryOutputStream(&store);
    }
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
    store.setMode(dagbase::BackingStore::MODE_INPUT_BIT);
    dagbase::InputStream *istr{nullptr};

    if (formatClass == "TextFormat")
    {
        istr = new dagbase::TextInputStream(&store);
    }
    else if (formatClass == "BinaryFormat")
    {
        istr = new dagbase::BinaryInputStream(&store);
    }
    ASSERT_NE(nullptr, istr);
    dagbase::Stream::ObjId id{~0U};
    auto actual = (istr->readRef<TestNode>(&id, lua));
    ASSERT_NE(nullptr, actual);
    ASSERT_NE(nullptr, actual->parent);
    EXPECT_EQ(42, actual->i);
    EXPECT_EQ(3141, actual->parent->i);
}

INSTANTIATE_TEST_SUITE_P(FormatAgnosticInputStream, FormatAgnosticOutputToInput_testRoundTrip, ::testing::Values(
    std::make_tuple("TextFormat"),
    std::make_tuple("BinaryFormat")
    ));

class OutputStream_testWriteVariant : public ::testing::TestWithParam<std::tuple<const char*, dagbase::Variant>>
{

};

TEST_P(OutputStream_testWriteVariant, testExpectedValue)
{
    std::string formatClass = std::get<0>(GetParam());
    dagbase::StreamFormat* format = nullptr;
    dagbase::MemoryBackingStore store(dagbase::BackingStore::MODE_OUTPUT_BIT);
    if (formatClass == "TextFormat")
    {
        format = new dagbase::TextFormat(&store);
    }
    else if (formatClass == "BinaryFormat")
    {
        format = new dagbase::BinaryFormat(&store);
    }
    ASSERT_NE(nullptr, format);
    dagbase::FormatAgnosticOutputStream sut(format, &store);
    sut.setFormat(format);
    sut.setBackingStore(&store);
    auto value = std::get<1>(GetParam());
    dagbase::Lua lua;
    sut.write(value);
    format->flush();
    dagbase::FormatAgnosticInputStream istr(format, &store);
    dagbase::Variant actualValue{std::numeric_limits<std::uint32_t>::max()};
    istr.read(lua, &actualValue);
    EXPECT_EQ(value, actualValue);
}

INSTANTIATE_TEST_SUITE_P(OutputStream, OutputStream_testWriteVariant, ::testing::Values(
        std::make_tuple("TextFormat", dagbase::Variant()),
        std::make_tuple("BinaryFormat", dagbase::Variant()),
        std::make_tuple("TextFormat", std::uint32_t{1}),
        std::make_tuple("BinaryFormat", std::uint32_t{1}),
        std::make_tuple("TextFormat", std::int64_t{-1}),
        std::make_tuple("BinaryFormat", std::int64_t{-1}),
        std::make_tuple("TextFormat", double{1.5}),
        std::make_tuple("BinaryFormat", double{1.5}),
        std::make_tuple("TextFormat", true),
        std::make_tuple("BinaryFormat", true),
        std::make_tuple("TextFormat", "test"),
        std::make_tuple("BinaryFormat", "test")
        ));

class OutputStream_testSerialisePrimitive : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

TEST_P(OutputStream_testSerialisePrimitive, testExpectedValue)
{
    std::string formatClass = std::get<0>(GetParam());
    dagbase::MemoryBackingStore store(dagbase::BackingStore::MODE_OUTPUT_BIT);
    dagbase::OutputStream* sut = nullptr;
    dagbase::InputStream* istr = nullptr;
    if (formatClass == "TextFormat")
    {
        sut = new dagbase::TextOutputStream(&store);
    }
    else if (formatClass == "BinaryFormat")
    {
        sut = new dagbase::BinaryOutputStream(&store);
    }
    store.setMode(dagbase::BackingStore::MODE_OUTPUT_BIT);
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
    if (formatClass == "TextFormat")
    {
        istr = new dagbase::TextInputStream(&store);
    }
    else if (formatClass == "BinaryFormat")
    {
        istr = new dagbase::BinaryInputStream(&store);
    }
    store.setMode(dagbase::BackingStore::MODE_INPUT_BIT);
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
    std::make_tuple("TextFormat"),
    std::make_tuple("BinaryFormat")
    ));

class InputStream_testReadEmptyBuffer : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

TEST_P(InputStream_testReadEmptyBuffer, testExpectedValue)
{
    dagbase::MemoryBackingStore store(dagbase::BackingStore::MODE_INPUT_BIT);
    std::string formatClass = std::get<0>(GetParam());
    dagbase::InputStream* sut = nullptr;
    if (formatClass == "TextFormat")
    {
        sut = new dagbase::TextInputStream(&store);
    }
    else if (formatClass == "BinaryFormat")
    {
        sut = new dagbase::BinaryInputStream(&store);
    }
    ASSERT_NE(nullptr, sut);
    std::int32_t actual{0};
    sut->readInt32(&actual);
    EXPECT_EQ(actual, 0);
}

INSTANTIATE_TEST_SUITE_P(InputStream, InputStream_testReadEmptyBuffer, ::testing::Values(
    std::make_tuple("TextFormat"),
    std::make_tuple("BinaryFormat")
    ));