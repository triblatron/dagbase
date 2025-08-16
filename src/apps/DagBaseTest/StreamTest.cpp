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

    TestNode(dagbase::InputStream& str)
    {
        str.addObj(this);
        std::string className;
        str.readHeader(&className);
        std::string fieldName;
        str.readField(&fieldName);
        dagbase::Stream::ObjId id{0};
        parent = str.readRef<TestNode>(&id);
        str.readField(&fieldName);
        str.readUInt32(&i);
        str.readField(&fieldName);
        str.readString(&s, true);
        str.readField(&fieldName);
        str.readBool(&b);
        str.readField(&fieldName);
        str.read(&value);
        std::uint32_t numChildren{0};
        str.readField(&fieldName);
        str.readUInt32(&numChildren);
        children.reserve(numChildren);
        str.readField(&fieldName);
        str.readHeader(&className);
        for (std::uint32_t i = 0; i < numChildren; ++i)
        {
            dagbase::Stream::ObjId childId{0};

            if (TestNode* child=str.readRef<TestNode>(&childId); child)
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
    node1.value = 2.0;
    node2.value = 1.0;
    dagbase::MemoryBackingStore store(dagbase::BackingStore::MODE_OUTPUT_BIT);

    dagbase::TextFormat format(&store);
    format.setMode(dagbase::StreamFormat::MODE_OUTPUT);
    dagbase::FormatAgnosticOutputStream sut(&format, &store);
    node2.write(sut);
    format.flush();

    std::string_view output = "TestNode\n{\n  parent : 1\n  TestNode\n  {\n    parent : 0\n    i : 0\n    s : \"\"\n    b : false\n    value : 2\n    numChildren : 0\n    children :     ChildrenArray\n    {\n    }\n  }\n  i : 0\n  s : \"test\"\n  b : true\n  value : 1\n  numChildren : 0\n  children :   ChildrenArray\n  {\n  }\n}\n";
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
    auto formatClass = std::get<0>(GetParam());
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
    TestNode node1;
    node1.i = 3141;
    TestNode node2;
    node2.i = 42;
    node2.parent = &node1;
    node1.children.emplace_back(&node2);
    if (sut.writeRef(&node2))
    {
        node2.write(sut);
    }
    format->flush();
    dagbase::FormatAgnosticInputStream istr(format, &store);
    dagbase::Stream::ObjId id{~0U};
    auto actual = (istr.readRef<TestNode>(&id));
    ASSERT_NE(nullptr, actual);
    ASSERT_NE(nullptr, actual->parent);
    EXPECT_EQ(42, actual->i);
    EXPECT_EQ(3141, actual->parent->i);
}

INSTANTIATE_TEST_SUITE_P(FormatAgnosticInputStream, FormatAgnosticOutputToInput_testRoundTrip, ::testing::Values(
    std::make_tuple("TextFormat"),
    std::make_tuple("BinaryFormat")
    ));
