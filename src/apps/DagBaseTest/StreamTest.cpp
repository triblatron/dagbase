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

class StreamFormat_testObject : public ::testing::TestWithParam<std::tuple<std::string_view, std::uint32_t>>
{

};

struct TestObject : public dagbase::Class
{
    std::uint32_t value{0};

    const char* className() const override
    {
        return "TestObject";
    }

    void writeToStream(dagbase::StreamFormat& format) const override
    {
        format.writeHeader("TestObject");
        Class::writeToStream(format);
        format.writeField("value");// << value << " }\n";
        format.writeUInt32(value);
        format.writeFooter();
    }

    void readFromStream(dagbase::StreamFormat& format) override
    {
        std::string className;
        format.readHeader(&className);
        if (className != "TestObject")
            return;
        Class::readFromStream(format);
        std::string fieldName;
        format.readField(&fieldName);
        if (fieldName != "value")
            return;
        format.readUInt32(&value);
        format.readFooter();
        // std::string className;
        //
        // str >> className;
        // if (className!="TestObject")
        //     return;
        // char temp;
        // str >> temp;
        // if (temp!='{')
        //     return;
        // Class::readFromStream(str);
        // std::string fieldName;
        // str >> fieldName;
        // if (fieldName!="value:")
        //     return;
        // str >> value;
        // str >> temp;
        // if (temp!='}')
        //     return;
    }
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

    TestNode() = default;

    TestNode(dagbase::InputStream& str)
    {
        std::string className;
        str.readHeader(&className);
        std::string fieldName;
        str.readField(&fieldName);
        dagbase::Stream::ObjId id{0};
        parent = str.readRef<TestNode>(&id);
        str.readField(&fieldName);
        str.readUInt32(&i);
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
    dagbase::MemoryBackingStore store(dagbase::BackingStore::MODE_OUTPUT_BIT);

    dagbase::TextFormat format(&store);
    format.setMode(dagbase::StreamFormat::MODE_OUTPUT);
    dagbase::FormatAgnosticOutputStream sut;
    sut.setFormat(&format);
    sut.setBackingStore(&store);
    node2.write(sut);
    format.flush();

    std::string_view output = "TestNode\n{\n  parent : 1\n  TestNode\n  {\n    parent : 0\n    i : 0\n  }\n  i : 0\n}\n";
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
    format->setMode(dagbase::StreamFormat::MODE_OUTPUT);
    dagbase::FormatAgnosticOutputStream sut;
    sut.setFormat(format);
    sut.setBackingStore(&store);
    TestNode node1;
    node1.i = 3141;
    TestNode node2;
    node2.i = 42;
    node2.parent = &node1;
    if (sut.writeRef(&node2))
    {
        node2.write(sut);
    }
    format->flush();
    store.setMode(dagbase::BackingStore::MODE_INPUT_BIT);
    dagbase::FormatAgnosticInputStream istr;
    format->setMode(dagbase::StreamFormat::MODE_INPUT);
    istr.setFormat(format);
    istr.setBackingStore(&store);
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
