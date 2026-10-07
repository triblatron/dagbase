//
// Created by Tony Horrobin on 28/04/2025.
//

#include "core/Variant.h"
#include "core/ConfigurationElement.h"
#include "core/LuaInterface.h"
#include "core/VariantArray.h"
#include "core/Value.h"
#include "io/MemoryBackingStore.h"
#include "io/TextOutputStream.h"
#include "io/TextInputStream.h"
#include "util/EnumValue.h"
#include "io/StreamFactory.h"
#include "core/TypeRegistry.h"
#include "core/Node.h"

#include "test/TestUtils.h"

#include <gtest/gtest.h>

#include <memory_resource>

#include "core/MetaClassRegistration.h"
#include "core/Variant.h"

class Variant_testToString : public ::testing::TestWithParam<std::tuple<dagbase::Variant, std::string>>
{

};

TEST_P(Variant_testToString, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto str = std::get<1>(GetParam());
    EXPECT_EQ(str, sut.toString());
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testToString, ::testing::Values(
        std::make_tuple("test", "test"),
        std::make_tuple(std::int64_t(1), "1"),
        std::make_tuple(1.5, "1.500000"),
        std::make_tuple(true, "1"),
        std::make_tuple(dagbase::Variant(dagbase::Colour{0.5f, 0.8f, 0.2f, 1.0f}), "Colour { 0.500000, 0.800000, 0.200000, 1.000000 }"),
        std::make_tuple(dagbase::Variant(dagbase::Vec2{ 1.5f, 10.0f }), "Vec2 { 1.500000, 10.000000 }")
        ));

class Variant_testAsInteger : public ::testing::TestWithParam<std::tuple<dagbase::Variant, std::int64_t>>
{

};

TEST_P(Variant_testAsInteger, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asInteger(0));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsInteger, ::testing::Values(
        std::make_tuple(std::int64_t(1), 1),
        std::make_tuple(1.5, 0),
        std::make_tuple("test", 0),
        std::make_tuple(true, 0),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, 0),
        std::make_tuple(dagbase::Vec2{1.0f,0.0f}, 0)
        ));
class Variant_testAsDouble : public ::testing::TestWithParam<std::tuple<dagbase::Variant, double>>
{

};

TEST_P(Variant_testAsDouble, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asDouble(0.0));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsDouble, ::testing::Values(
        std::make_tuple(std::int64_t(1), 0.0),
        std::make_tuple(1.5, 1.5),
        std::make_tuple("test", 0),
        std::make_tuple(true, 0),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, 0),
        std::make_tuple(dagbase::Vec2{1.0f,0.0f}, 0)
        ));

class Variant_testAsBool : public ::testing::TestWithParam<std::tuple<dagbase::Variant, bool>>
{

};

TEST_P(Variant_testAsBool, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asBool(false));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsBool, ::testing::Values(
        std::make_tuple(std::int64_t(1), false),
        std::make_tuple(1.5, false),
        std::make_tuple("test", false),
        std::make_tuple(true, true),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, false),
        std::make_tuple(dagbase::Vec2{1.0f,0.0f}, false)
        ));


class Variant_testAsString : public ::testing::TestWithParam<std::tuple<dagbase::Variant, std::string>>
{

};

TEST_P(Variant_testAsString, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asString(""));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsString, ::testing::Values(
        std::make_tuple(std::int64_t(1), ""),
        std::make_tuple(1.5, ""),
        std::make_tuple("test", "test"),
        std::make_tuple(true, ""),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, ""),
        std::make_tuple(dagbase::Vec2{1.0f,0.0f}, "")
        ));
class Variant_testAsColour : public ::testing::TestWithParam<std::tuple<dagbase::Variant, dagbase::Colour>>
{

};

TEST_P(Variant_testAsColour, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asColour(dagbase::Colour{0.0f,0.0f,0.0f,0.0f}));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsColour, ::testing::Values(
        std::make_tuple(std::int64_t(1), dagbase::Colour{0.0f,0.0f,0.0f,0.0f}),
        std::make_tuple(1.5, dagbase::Colour{0.0f,0.0f,0.0f,0.0f}),
        std::make_tuple("test", dagbase::Colour{0.0f,0.0f,0.0f,0.0f}),
        std::make_tuple(true, dagbase::Colour{0.0f,0.0f,0.0f,0.0f}),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, dagbase::Colour{1.0f,1.0f,1.0f,1.0f}),
        std::make_tuple(dagbase::Vec2{1.0f,0.0f}, dagbase::Colour{0.0f,0.0f,0.0f,0.0f})
        ));

class Variant_testAsVec2 : public ::testing::TestWithParam<std::tuple<dagbase::Variant, dagbase::Vec2>>
{

};

TEST_P(Variant_testAsVec2, testExpectedValue)
{
    auto sut = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(value, sut.asVec2(dagbase::Vec2{0.0f,0.0f}));
}

INSTANTIATE_TEST_SUITE_P(Variant, Variant_testAsVec2, ::testing::Values(
        std::make_tuple(std::int64_t(1), dagbase::Vec2{0.0f,0.0f}),
        std::make_tuple(1.5, dagbase::Vec2{0.0f,0.0f}),
        std::make_tuple("test", dagbase::Vec2{0.0f,0.0f}),
        std::make_tuple(true, dagbase::Vec2{0.0f,0.0f}),
        std::make_tuple(dagbase::Colour{1.0f,1.0f,1.0f,1.0f}, dagbase::Vec2{0.0f,0.0f}),
        std::make_tuple(dagbase::Vec2{1.0f,1.0f}, dagbase::Vec2{1.0f,1.0f})
        ));

class VariantArray_testConfigure : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(VariantArray_testConfigure, testExpectedValue)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::VariantArray sut;
    sut.configure(*config);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actualValue = sut.find(path);
    assertComparison(value, actualValue, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(VariantArray, VariantArray_testConfigure, ::testing::Values(
        std::make_tuple("data/tests/VariantArray/Int64.lua", "items[0]", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/VariantArray/Int64.lua", "items[1]", std::int64_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/VariantArray/Double.lua", "items[0]", 2.5, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/VariantArray/Bool.lua", "items[0]", false, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/VariantArray/Bool.lua", "items[1]", true, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/VariantArray/String.lua", "items[0]", std::string{"test1"}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
//        std::make_tuple("data/tests/VariantArray/Colour.lua", "items[0]", dagbase::Colour{0.1f, 0.2f, 0.4f, 1.0f}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        std::make_tuple("data/tests/VariantArray/UInt32.lua", "items[0]", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/VariantArray/UInt32.lua", "items[1]", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

class VariantArray_testEmplaceBack : public ::testing::TestWithParam<std::tuple<dagbase::Variant>>
{

};

TEST_P(VariantArray_testEmplaceBack, testExpectedValue)
{
    auto value = std::get<0>(GetParam());
    dagbase::VariantArray sut;
    sut.emplace_back(value);
    EXPECT_EQ(value, sut[0]);
}

INSTANTIATE_TEST_SUITE_P(VariantArray, VariantArray_testEmplaceBack, ::testing::Values(
        std::make_tuple(std::int64_t{1}),
        std::make_tuple(2.5),
        std::make_tuple(std::string("test")),
        std::make_tuple(false),
        std::make_tuple(true),
        std::make_tuple(std::uint32_t{2})
        ));

class Value_testPushBack : public ::testing::TestWithParam<std::tuple<dagbase::Value>>
{

};

TEST_P(Value_testPushBack, testExpectedValue)
{
    dagbase::Value sut;
    auto value = std::get<0>(GetParam());

    sut.push_back(value);
    ASSERT_FALSE(sut.empty());
    ASSERT_EQ(value, sut[1]);
}

INSTANTIATE_TEST_SUITE_P(Value, Value_testPushBack, ::testing::Values(
    std::make_tuple(dagbase::Value(std::uint8_t{1})),
    std::make_tuple(dagbase::Value(std::int8_t{1})),
    std::make_tuple(dagbase::Value(std::uint16_t{1})),
    std::make_tuple(dagbase::Value(std::int16_t{1})),
    std::make_tuple(dagbase::Value(std::uint32_t{1})),
    std::make_tuple(dagbase::Value(std::int32_t{1})),
    std::make_tuple(dagbase::Value(std::uint64_t{1})),
    std::make_tuple(dagbase::Value(std::int64_t{1})),
    std::make_tuple(dagbase::Value(float(1.5f))),
    std::make_tuple(dagbase::Value(double(1.5))),
    std::make_tuple(dagbase::Value(new std::string("test"))),
    std::make_tuple(dagbase::Value(true)),
    std::make_tuple(dagbase::Value(dagbase::Vec2{1.0f,2.0f})),
    std::make_tuple(dagbase::Value((void*)nullptr)),
    std::make_tuple(dagbase::Value(new std::vector<dagbase::Value>{dagbase::Value{1},dagbase::Value{2},dagbase::Value{3}}))
    ));

class Value_testSerialise : public ::testing::TestWithParam<std::tuple<dagbase::Value>>
{

};

TEST_P(Value_testSerialise, testExpectedValue)
{
    auto value = std::get<0>(GetParam());
    auto* buf = new dagbase::MemoryBackingStore();
    buf->open(dagbase::BackingStore::MODE_OUTPUT_BIT, "");
    dagbase::TextOutputStream ostr(buf);
    value.writeToStream(ostr);
    ostr.flush();
    buf->open(dagbase::BackingStore::MODE_INPUT_BIT, "");
    dagbase::TextInputStream istr(buf);
    dagbase::Value actual;
    actual.readFromStream(istr);
    EXPECT_EQ(value, actual);
}

INSTANTIATE_TEST_SUITE_P(Value, Value_testSerialise, ::testing::Values(
    std::make_tuple(dagbase::Value(std::uint8_t{1})),
    std::make_tuple(dagbase::Value(std::int8_t{1})),
    std::make_tuple(dagbase::Value(std::uint16_t{1})),
    std::make_tuple(dagbase::Value(std::int16_t{1})),
    std::make_tuple(dagbase::Value(std::uint32_t{1})),
    std::make_tuple(dagbase::Value(std::int32_t{1})),
    std::make_tuple(dagbase::Value(std::uint64_t{1})),
    std::make_tuple(dagbase::Value(std::int64_t{1})),
    std::make_tuple(dagbase::Value(1.5f)),
    std::make_tuple(dagbase::Value(1.5)),
    std::make_tuple(dagbase::Value(new std::string("test"))),
    std::make_tuple(dagbase::Value(false)),
    std::make_tuple(dagbase::Value(true)),
    std::make_tuple(dagbase::Value(dagbase::Vec2())),
    std::make_tuple(dagbase::Value(new std::vector<dagbase::Value>({dagbase::Value(std::uint32_t{100})})))
    ));

class Value_testEnum : public ::testing::TestWithParam<std::tuple<dagbase::Value::Type, bool>>
{

};

TEST_P(Value_testEnum, testExpectedValue)
{
    auto value = std::get<0>(GetParam());
    auto equal = std::get<1>(GetParam());
    dagbase::Value sut(value);
    EXPECT_EQ(equal, sut==dagbase::Value(value));
}

INSTANTIATE_TEST_SUITE_P(Value, Value_testEnum, ::testing::Values(
    std::make_tuple(dagbase::Value::TYPE_INT32, true)
    ));

class EnumValue_testSet : public ::testing::TestWithParam<std::tuple<dagbase::Variant::Index, bool>>
{

};

const char* variantTypeToString(std::uint32_t value)
{
    return dagbase::Variant::indexToString(static_cast<dagbase::Variant::Index>(value));
}

std::uint32_t parseVariantType(const char* str)
{
    return dagbase::Variant::parseIndex(str);
}

void createIndex(dagbase::Type& type)
{
    type.name = dagbase::Atom::intern("Index");
    type.unknownValue = dagbase::Variant::TYPE_UNKNOWN;
    type.minValue = dagbase::Variant::TYPE_INTEGER;
    type.maxValue = dagbase::Variant::TYPE_VALUE;
    type.toString = [](std::uint32_t value) {
        return dagbase::Variant::indexToString(static_cast<dagbase::Variant::Index>(value));
    };
    type.parse = [](const std::string& str) {
        return (dagbase::Variant::parseIndex(str.c_str()));
    };
    type.flags = dagbase::Type::FLAGS_NONE;
}

void createNodeFlags(dagbase::Type& type)
{
    type.name = dagbase::Atom::intern("NodeFlags");
    type.unknownValue = dagbase::Node::NodeFlags::NODE_NONE;
    type.flags = dagbase::Type::BITMASK_BIT;
    type.minValue = dagbase::Node::NodeFlags::NODE_NONE;
    type.maxValue = 4;
    type.toString = [](std::uint32_t value) {
        return dagbase::Node::flagsToString(static_cast<dagbase::Node::NodeFlags>(value));
    };
    type.parse = [](const std::string& str) {
        return dagbase::Node::parseFlags(str);
    };
}

void createNodeActive(dagbase::Type& type)
{
    type.name = dagbase::Atom::intern("NodeActive");
    type.base = dagbase::TypeRegistry::getTypeRegistry().findType(dagbase::Atom::intern("NodeFlags"));
    type.unknownValue = dagbase::Node::ACTIVE_ON;
    type.flags = dagbase::Type::SUBFIELD_BIT;
    type.minValue = dagbase::Node::ACTIVE_ON;
    type.maxValue = dagbase::Node::ACTIVE_PASS_THROUGH;
    type.shift = dagbase::Node::SHIFT_ACTIVE;
    type.mask = dagbase::Node::MASK_ACTIVE;

    type.toString = [](std::uint32_t value) {
        return dagbase::Node::activeToString(static_cast<dagbase::Node::Active>(value));
    };
    type.parse = [](const std::string& str) {
        return dagbase::Node::parseActive(str.c_str());
    };
}

TEST_P(EnumValue_testSet, testHeld)
{
    dagbase::Variant::Index value = std::get<0>(GetParam());
    auto exists = std::get<1>(GetParam());
    dagbase::Type type;
    createIndex(type);
    dagbase::EnumValue sut(&type);
    sut.set(value);
    ASSERT_EQ(value, sut.get<dagbase::Variant::Index>());
}

INSTANTIATE_TEST_SUITE_P(EnumValue, EnumValue_testSet, ::testing::Values(
    std::make_tuple(dagbase::Variant::TYPE_DOUBLE, true)
    ));

TEST(EnumValue_testSet, testUnknown)
{
    dagbase::Type type;
    createIndex(type);
    dagbase::EnumValue sut(&type);
    sut.set("TYPE_SPOO");
    ASSERT_EQ(dagbase::Variant::TYPE_UNKNOWN, sut.get<dagbase::Variant::Index>());
}

TEST(EnumValue, testNoParser)
{
    dagbase::EnumValue sut;
    ASSERT_EQ("<error>", sut.toString());
    ASSERT_EQ(0, sut.get<dagbase::Variant::Index>());
}

TEST(Value, Value_setEnum)
{
    dagbase::Type type;
    createIndex(type);
    dagbase::Value sut;
    auto enumValue = dagbase::EnumValue(&type);
    enumValue.set("TYPE_DOUBLE");
    sut = dagbase::Value(enumValue);
    auto actual = std::get<dagbase::EnumValue>(sut.value());
    EXPECT_EQ(dagbase::Variant::TYPE_DOUBLE, actual.get<dagbase::Variant::Index>());
}

struct VarintCase
{
    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig(config, "value", &value);
        dagbase::ConfigurationElement::readConfig(config, "moreBit", &moreBit);
        if (auto element=config.findElement("bytes"); element)
        {
            element->eachChild([this](dagbase::ConfigurationElement& child)  {
                dagbase::Value v = child.value().asValueInteger(dagbase::Value::TYPE_UINT8, dagbase::Value(std::uint8_t{0}));
                if (v.type() == dagbase::Value::TYPE_UINT8)
                    bytes.emplace_back(std::get<std::uint8_t>(v.value()));

                return true;
            });
        }
    }

    static void buildFilename(std::uint32_t caseIndex, const char* storeClass, const char* formatClass, std::string* value)
    {
        if (value)
        {
            std::ostringstream str;
            str << "scratch/Value_testVarint_case" << caseIndex << '_' << storeClass << '_' << formatClass;
            if (std::strcmp(formatClass, "TextFormat")==0)
            {
                str << ".txt";
            }
            else if (std::strcmp(formatClass, "BinaryFormat")==0)
            {
                str << ".bin";
            }
            else
            {
                FAIL() << "Expected TextFormat or BinaryFormat, got " << formatClass;
            }
            *value = str.str();
        }
    }

    void makeItSo(std::size_t caseIndex, const char* storeClass, const char* formatClass) const
    {
        std::uint8_t moreMask = 1<<moreBit;
        std::uint8_t mask = moreMask | (moreMask - 1);
        dagbase::Value asValue = value.asValueInteger(dagbase::Value::TYPE_UINT64, dagbase::Value(std::uint64_t{0}));
        std::byte buf[8];
        std::pmr::monotonic_buffer_resource pool{buf, sizeof(buf)};
        std::pmr::vector<std::uint8_t> actual{&pool};
        asValue.varintEncode(moreBit, &actual);
        ASSERT_EQ(bytes.size(), actual.size()) << "Case " << caseIndex << ":Expected " << bytes.size() << " bytes, got " << actual.size();
        for (std::size_t byteIndex=0; byteIndex<bytes.size(); ++byteIndex)
        {
            EXPECT_EQ(bytes[byteIndex], actual[byteIndex]) << "Case " << caseIndex << ":Expected byte " << byteIndex << " to be " << (std::uint32_t)bytes[byteIndex] << ", got " << (std::uint32_t)actual[byteIndex];
        }
        dagbase::Value decoded = dagbase::Value::fromVarint(moreBit, actual);
        EXPECT_EQ(asValue, decoded) << "Case " << caseIndex << ":Expected decoded to be equal to value";
        // Serialise
        dagbase::BackingStore* store = dagbase::createBackingStore(storeClass);
        ASSERT_NE(nullptr, store);
        std::string filename;
        buildFilename(caseIndex, storeClass, formatClass, &filename);
        dagbase::OutputStream* ostr = dagbase::createOutputStream(formatClass, *store, filename.c_str());
        ASSERT_NE(nullptr, ostr);
        dagbase::Lua lua;
        ostr->writeVariableLengthInteger(actual);
        ostr->flush();

        // Deserialise
        dagbase::InputStream *istr = dagbase::createInputStream(formatClass, *store, filename.c_str());
        ASSERT_NE(nullptr, istr);
        dagbase::Value actualValue{};
        std::byte streamBuf[8];
        std::pmr::monotonic_buffer_resource streamPool{buf, sizeof(buf)};
        std::uint64_t actualFromStream{0};
        istr->readVariableLengthInteger(moreBit, &actualFromStream);
        EXPECT_EQ(std::uint64_t(asValue), actualFromStream) << "Case " << caseIndex << ":Expected deserialised to be equal to value";
    }

    dagbase::Variant value;
    // Bit index of the more bit, zero-based starting from the lsb
    std::uint32_t moreBit{0};
    std::vector<std::uint8_t> bytes;
};

class Value_testVarint : public ::testing::TestWithParam<std::tuple<const char*>>
{
public:
    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfigVector(config, "cases", &_cases);
    }

    void makeItSo()
    {
        for (std::size_t i=0; i<_cases.size(); ++i)
        {
            _cases[i].makeItSo(i, "MemoryBackingStore", "TextFormat");
            _cases[i].makeItSo(i, "MemoryBackingStore", "BinaryFormat");
            _cases[i].makeItSo(i, "FileBackingStore", "TextFormat");
            _cases[i].makeItSo(i, "FileBackingStore", "BinaryFormat");
        }
    }
protected:
    using CaseArray = std::vector<VarintCase>;
    CaseArray _cases;
};

TEST_P(Value_testVarint, testExpectedValue)
{
    auto configFilename = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configFilename);
    ASSERT_NE(nullptr, config);
    configure(*config);
    makeItSo();
}

INSTANTIATE_TEST_SUITE_P(Value, Value_testVarint, ::testing::Values(
    std::make_tuple("data/tests/Varint/Varint.lua")
    ));

TEST(TypeRegistry, testRegisterEnum)
{
    dagbase::Type testType;
    createIndex(testType);
    auto id = dagbase::TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("Index"), &testType);
    ASSERT_GT(id, 0);
    dagbase::EnumValue enumValue(&testType);
    enumValue.set("TYPE_DOUBLE");
    dagbase::Value sut = dagbase::Value(enumValue);
    dagbase::MemoryBackingStore store;
    dagbase::OutputStream* ostr = dagbase::createOutputStream("TextFormat", store, "");
    ASSERT_NE(nullptr, ostr);
    sut.writeToStream(*ostr);
    ostr->flush();
    auto istr = dagbase::createInputStream("TextFormat", store, "");
    ASSERT_NE(nullptr, istr);
    dagbase::Value actual;
    actual.readFromStream(*istr);
    EXPECT_EQ(sut, actual);
    dagbase::TypeRegistry::getTypeRegistry().unregisterType(testType.name);
}

struct EnumerateNameValue
{
    dagbase::Atom name;
    dagbase::Variant value;
    std::uint32_t mask{0};
    std::uint32_t shift{0};

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig(config, "name", &name);
        dagbase::ConfigurationElement::readConfig(config, "value", &value);
        dagbase::ConfigurationElement::readConfig(config, "mask", &mask);
        dagbase::ConfigurationElement::readConfig(config, "shift", &shift);
    }
};

struct EnumerateCase
{
    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig(config, "name", &name);
        type = dagbase::TypeRegistry::getTypeRegistry().findType(name);
        dagbase::ConfigurationElement::readConfigVector(config, "values", &values);
    }

    void makeItSo() const
    {
        ASSERT_NE(nullptr, type);
        auto value = (type->minValue<<type->shift)&type->mask;
        auto shifted = type->flags & dagbase::Type::BITMASK_BIT?1<<value:value;
        std::cerr << "minValue: " << type->minValue << ", maxValue: " << type->maxValue << '\n';
        ASSERT_EQ(values.size(), (type->maxValue-type->minValue)+1);
        ASSERT_TRUE(type->toString);
        ASSERT_TRUE(type->parse);
        for (std::uint32_t i=0; i<(type->maxValue-type->minValue); ++i)
        {
            auto expectedValue = values[i].value.cast(dagbase::Variant::TYPE_UINT).asUint32();
            ASSERT_EQ(expectedValue, shifted);
            if (type->flags & dagbase::Type::SUBFIELD_BIT)
            {
                auto extracted = (expectedValue&type->mask)>>type->shift;
                ASSERT_EQ(values[i].name.toString(), type->toString(extracted));
                ASSERT_EQ(extracted, type->parse(values[i].name.toString()));
            }
            else
            {
                ASSERT_EQ(values[i].name.toString(), type->toString(shifted));
                ASSERT_EQ(shifted, type->parse(values[i].name.toString()));
            }
            auto nextValue = type->nextValue(value);
            value = nextValue;
            shifted = type->flags & dagbase::Type::BITMASK_BIT?1<<value:value;
            ASSERT_NE(value, type->unknownValue);
        }
        value = type->nextValue(value);
        ASSERT_EQ(type->unknownValue, value);
    }

    dagbase::Type* type{nullptr};
    dagbase::Atom name;
    using NameValueArray = std::vector<EnumerateNameValue>;
    NameValueArray values;
};

class Enum_testEnumerate : public ::testing::TestWithParam<std::tuple<const char*>>
{
public:
    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfigVector(config, "cases", &_cases);
    }

    void makeItSo() const
    {
        for (const auto& testCase : _cases)
        {
            testCase.makeItSo();
        }
    }
protected:
    void SetUp() override
    {
        indexType = new dagbase::Type();
        createIndex(*indexType);
        dagbase::TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("Index"), indexType);
        nodeFlagsType = new dagbase::Type();
        createNodeFlags(*nodeFlagsType);
        dagbase::TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("NodeFlags"), nodeFlagsType);
        nodeActiveType = new dagbase::Type();
        createNodeActive(*nodeActiveType);
        dagbase::TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("NodeActive"), nodeActiveType);
    }

    void TearDown() override
    {
        dagbase::TypeRegistry::getTypeRegistry().unregisterType(indexType->name);
        dagbase::TypeRegistry::getTypeRegistry().unregisterType(nodeFlagsType->name);
        dagbase::TypeRegistry::getTypeRegistry().unregisterType(nodeActiveType->name);
        delete indexType;
        delete nodeFlagsType;
        delete nodeActiveType;
    }

    dagbase::Type* indexType{nullptr};
    dagbase::Type* nodeFlagsType{nullptr};
    dagbase::Type* nodeActiveType{nullptr};
    using EnumerateCaseArray = std::vector<EnumerateCase>;
    EnumerateCaseArray _cases;
};

TEST_P(Enum_testEnumerate, testExpectedNumber)
{
    auto configFilename = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configFilename);
    ASSERT_NE(nullptr, config);
    configure(*config);
    makeItSo();
}

INSTANTIATE_TEST_SUITE_P(Enum, Enum_testEnumerate, ::testing::Values(
    std::make_tuple("data/tests/Enum/Index.lua"),
    std::make_tuple("data/tests/Enum/NodeFlags.lua")
    ));
