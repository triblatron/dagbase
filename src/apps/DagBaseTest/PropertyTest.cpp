//
// Created by Tony Horrobin on 27/04/2025.
//

#include "core/MetaProperty.h"
#include "core/ConfigurationElement.h"
#include "core/LuaInterface.h"
#include "test/TestUtils.h"
#include "core/PropertyBagFactory.h"
#include "core/PropertyBag.h"
#include "core/MetaClass.h"
#include "core/Property.h"
#include "core/ClassDescription.h"
#include "core/Signal.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


class MetaProperty_testConfigure : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::ConfigurationElement::ValueType, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(MetaProperty_testConfigure, testExpectedValues)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::MetaProperty sut;
    sut.configure(*config);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actualValue = sut.find(path);
    assertComparison(value, actualValue, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(MetaProperty, MetaProperty_testConfigure, ::testing::Values(
        std::make_tuple("data/tests/MetaProperty/NoSemantic.lua", "dataType", "DATATYPE_STRING", 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/MetaProperty/NoSemantic.lua", "format", "FORMAT_NONE", 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/MetaProperty/Colour.lua", "dataType", "DATATYPE_STRING", 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/MetaProperty/Colour.lua", "format", "FORMAT_COLOUR", 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

class MetaPropertyDataType_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::MetaProperty::DataType>>
{

};

TEST_P(MetaPropertyDataType_testRoundTrip, testExpectedValue)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::MetaProperty::dataTypeToString(value));
    EXPECT_EQ(value, dagbase::MetaProperty::parseDataType(str));
}

INSTANTIATE_TEST_SUITE_P(MetaPropertyDataType, MetaPropertyDataType_testRoundTrip, ::testing::Values(
        std::make_tuple("DATATYPE_UNKNOWN", dagbase::MetaProperty::DATATYPE_UNKNOWN),
        std::make_tuple("DATATYPE_STRING", dagbase::MetaProperty::DATATYPE_STRING),
        std::make_tuple("DATATYPE_INT", dagbase::MetaProperty::DATATYPE_INT),
        std::make_tuple("DATATYPE_BOOL", dagbase::MetaProperty::DATATYPE_BOOL),
        std::make_tuple("DATATYPE_DOUBLE", dagbase::MetaProperty::DATATYPE_DOUBLE)
        ));

class MetaPropertyFormat_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::MetaProperty::Format>>
{

};

TEST_P(MetaPropertyFormat_testRoundTrip, testExpectedValue)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::MetaProperty::formatToString(value));
    EXPECT_EQ(value, dagbase::MetaProperty::parseFormat(str));
}

INSTANTIATE_TEST_SUITE_P(MetaPropertyFormat, MetaPropertyFormat_testRoundTrip, ::testing::Values(
        std::make_tuple("FORMAT_UNKNOWN", dagbase::MetaProperty::FORMAT_UNKNOWN),
        std::make_tuple("FORMAT_NONE", dagbase::MetaProperty::FORMAT_NONE),
        std::make_tuple("FORMAT_COLOUR", dagbase::MetaProperty::FORMAT_COLOUR)
        ));

struct TestProperties
{
    std::int64_t i{0};

    dagbase::Variant property(std::string_view path) const
    {
        dagbase::Variant retval;

        retval = dagbase::findEndpoint(path, "i", i);
        if (retval.has_value())
            return retval;

        return {};
    }

    void configure(dagbase::ConfigurationElement& config)
    {
        if (auto element = config.findElement("i"); element)
        {
            i = element->asInteger();
        }
    }
};

class PropertyBag_testLookup : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(PropertyBag_testLookup, testExpectedValue)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::PropertyBagFactory factory;
    dagbase::TemplatePropertyBag<TestProperties> *foo = new dagbase::TemplatePropertyBag<TestProperties>();
    delete foo;
    factory.setUnknownClassHandler([this](const std::string& className, dagbase::ConfigurationElement& config) {
        if (className == "TestPropertyBag")
        {
            return static_cast<dagbase::PropertyBag*>(new dagbase::TemplatePropertyBag<TestProperties>());
        }
        return static_cast<dagbase::PropertyBag*>(nullptr);
    });
    auto sut = factory.create(*config);
    ASSERT_NE(nullptr, sut);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actualValue = sut->property(path);
    assertComparison(value, actualValue, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(PropertyBag, PropertyBag_testLookup, ::testing::Values(
        std::make_tuple("data/tests/PropertyBag/TestProperties.lua", "i", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/PropertyBag/TestProperties.lua", "spoo", true, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

class TestObjectWithProperties
{
    DAGBASE_PROPERTY(int, foo, _foo, 0, setFoo, foo)
};

TEST(PropertyDecl, testReadWrite)
{
    TestObjectWithProperties sut;
    sut.setFoo(1);
    EXPECT_EQ(1, sut.foo());
}

#define DECLARE_PROPERTY_READER(type, name, member) \
    type name() const \
    {\
        return member;\
    }

#define DECLARE_PROPERTY_WRITER(type, name, member) \
    void name(type value) \
    {\
        member = value;\
    }

class TestClassWithProperties
{
public:
    DECLARE_PROPERTY_READER(bool, foo, _foo)
    DECLARE_PROPERTY_WRITER(bool, setFoo, _foo)

    // Signals
    dagbase::Signal<void()> bar;

    // Slots
    void onBar();
private:
    bool _foo{false};
};

TEST(PropertyDecl, testAccessors)
{
    TestClassWithProperties sut;

    bool actual = sut.foo();
    ASSERT_FALSE(actual);
    sut.setFoo(true);
    ASSERT_TRUE(sut.foo());
}

class TestOwner : public dagbase::Class
{
public:
    void setValue(const std::int32_t& value)
    {
        _value = value;
    }

    void getValue(std::int32_t* value) const
    {
        if (value)
        {
            *value = _value;
        }
    }

    void valueChanged()
    {
        // Do nothing.
    }

    void describe(dagbase::ClassDescription& description) const override
    {
        description.addInt32(dagbase::Atom::intern("value"));
    }
private:
    std::int32_t _value{0};
};

class Property_testAccessors : public ::testing::TestWithParam<std::tuple<const char*, std::int32_t>>
{

};

TEST_P(Property_testAccessors, testExpectedValue)
{
    auto name = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    TestOwner owner;
    dagbase::Property sut(dagbase::Atom::intern(name), &TestOwner::getValue, &TestOwner::setValue, &TestOwner::valueChanged);
    sut.set(owner, value);
    std::int32_t actual = 0;
    sut.get(&owner, &actual, sizeof(std::int32_t));
    ASSERT_EQ(value, actual);
}

INSTANTIATE_TEST_SUITE_P(Property, Property_testAccessors, ::testing::Values(
    std::make_tuple("value", 1)
    ));

class TestClassDescription : public dagbase::ClassDescription
{
public:
    MOCK_METHOD(void, addInt8, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addInt16, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addInt32, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addInt64, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addFloat, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addDouble, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addString, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addBool, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addUint8, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addUint16, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addUint32, (const dagbase::Atom& name), (override));
    MOCK_METHOD(void, addUint64, (const dagbase::Atom& name), (override));
};

TEST(Class, testDescribe)
{
    TestOwner sut;
    TestClassDescription description;
    EXPECT_CALL(description, addInt32(::testing::_)).Times(1);
    sut.describe(description);
}