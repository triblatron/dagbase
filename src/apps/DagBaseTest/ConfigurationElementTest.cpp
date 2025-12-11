#include "config/config.h"

#include "core/ConfigurationElement.h"
#include "core/LuaInterface.h"
#include "io/StreamFormat.h"
#include "io/MemoryBackingStore.h"
#include "io/BinaryFormat.h"
#include "io/TextFormat.h"
#include "io/FormatAgnosticOutputStream.h"
#include "io/FormatAgnosticInputStream.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class ConfigurationElement_testFindElement : public ::testing::TestWithParam<std::tuple<const char*, const char*, const char*>>
{

};

TEST_P(ConfigurationElement_testFindElement, testFindFromRoot)
{
    dagbase::Lua lua;
    auto configStr = std::get<0>(GetParam());
    auto path = std::get<1>(GetParam());
    auto name = std::get<2>(GetParam());
    auto config = dagbase::ConfigurationElement::fromString(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto actual = config->findElement(path);
    ASSERT_NE(nullptr, actual);
    ASSERT_EQ(name, actual->name());

    delete config;
}

INSTANTIATE_TEST_SUITE_P(ConfigurationElement, ConfigurationElement_testFindElement, ::testing::Values(
        std::make_tuple("root = {}", "$", "root"),
        std::make_tuple("root = { foo = true }", "$.foo", "foo"),
        std::make_tuple("root = { foo = { wibble=1.0 } }", "$.foo.wibble", "wibble"),
        std::make_tuple("root = { foo = { flibble={ spoo=3 } } }", "$.foo.flibble.spoo", "spoo"),
        std::make_tuple("root = { foo = true }", "foo", "foo"),
        std::make_tuple("root = { foo = { bar=\"wibble\" } }", "foo.bar", "bar"),
        std::make_tuple("root = { foo = { bar={ baz=2 } } }", "foo.bar.baz", "baz"),
        std::make_tuple("root = { \"wibble\" }", "$[0]", ""),
        std::make_tuple("root = { wibble={ true } }", "$.wibble[0]", ""),
        std::make_tuple("root = { wibble={ { foo=true } } }", "$.wibble[0].foo", "foo"),
        std::make_tuple("root = { wibble={ { foo={true} } } }", "$.wibble[0].foo[0]", ""),
        std::make_tuple("root = { wibble={ foo={ true } } }", "$.wibble.foo[0]", ""),
        std::make_tuple("root = { wibble={ foo={ { bar=1.0 } } } }", "$.wibble.foo[0].bar", "bar"),
        std::make_tuple("root = { wibble={ foo={ { bar=1.0 }, { baz=\"baz\" }, } } }", "$.wibble.foo[1].baz", "baz"),
        std::make_tuple("root = { wibble={ foo={ true } }, flibble={ tribble=1.0 } }", "$.flibble.tribble", "tribble"),
        std::make_tuple("root = { wibble={ foo={ true } }, flibble={ tribble=1.0 } }", "wibble.foo", "foo"),
        std::make_tuple("root = { wibble={ { foo=true } }, { tribble=1.0 } }", "wibble[0].foo", "foo"),
        std::make_tuple("root = { { foo=true }, { tribble=1.0 } }", "$[1].tribble", "tribble"),
        std::make_tuple("root = { { foo=true }, { tribble=1.0 } }", "[1].tribble", "tribble"),
        std::make_tuple("root = { wibble={ { foo=true }, { tribble=1.0 }, } }", "wibble[1].tribble", "tribble")
        ));

class ConfigurationElement_testAsInteger : public ::testing::TestWithParam<std::tuple<const char*, const char*, std::int64_t>>
{

};

TEST_P(ConfigurationElement_testAsInteger, testAsInteger)
{
    dagbase::Lua lua;
    auto configStr = std::get<0>(GetParam());
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());

    auto config = dagbase::ConfigurationElement::fromString(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto element = config->findElement(path);
    ASSERT_NE(nullptr, element);
    EXPECT_EQ(value, element->asInteger());
    delete config;
}

INSTANTIATE_TEST_SUITE_P(ConfigurationElement, ConfigurationElement_testAsInteger, ::testing::Values(
        std::make_tuple("root={ foo=1 }", "foo", 1),
        std::make_tuple("root={ foo={ 1 } }", "foo[0]", 1)
        ));

class ConfigurationElement_testAsDouble : public ::testing::TestWithParam<std::tuple<const char*, const char*, double>>
{

};

TEST_P(ConfigurationElement_testAsDouble, testAsDouble)
{
    dagbase::Lua lua;
    auto configStr = std::get<0>(GetParam());
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());

    auto config = dagbase::ConfigurationElement::fromString(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto element = config->findElement(path);
    ASSERT_NE(nullptr, element);
    EXPECT_EQ(value, element->asDouble());
    delete config;
}

INSTANTIATE_TEST_SUITE_P(ConfigurationElement, ConfigurationElement_testAsDouble, ::testing::Values(
        std::make_tuple("root={ foo=1.5 }", "foo", 1.5),
        std::make_tuple("root={ foo= { 1.5 } }", "foo[0]", 1.5)
        ));

class ConfigurationElement_testAsBool : public ::testing::TestWithParam<std::tuple<const char*, const char*, bool>>
{

};

TEST_P(ConfigurationElement_testAsBool, testAsBool)
{
    dagbase::Lua lua;
    auto configStr = std::get<0>(GetParam());
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());

    auto config = dagbase::ConfigurationElement::fromString(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto element = config->findElement(path);
    ASSERT_NE(nullptr, element);
    EXPECT_EQ(value, element->asBool());
    delete config;
}

INSTANTIATE_TEST_SUITE_P(ConfigurationElement, ConfigurationElement_testAsBool, ::testing::Values(
        std::make_tuple("root={ foo=true }", "foo", true),
        std::make_tuple("root={ foo= { true } }", "foo[0]", true)
        ));

class ConfigurationElement_testAsString : public ::testing::TestWithParam<std::tuple<const char*, const char*, const char*>>
{

};

TEST_P(ConfigurationElement_testAsString, testAsString)
{
    dagbase::Lua lua;
    auto configStr = std::get<0>(GetParam());
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());

    auto config = dagbase::ConfigurationElement::fromString(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto element = config->findElement(path);
    ASSERT_NE(nullptr, element);
    EXPECT_EQ(value, element->asString());
    delete config;
}

INSTANTIATE_TEST_SUITE_P(ConfigurationElement, ConfigurationElement_testAsString, ::testing::Values(
        std::make_tuple("root={ foo=\"true\" }", "foo", "true"),
        std::make_tuple("root={ foo= { \"true\" } }", "foo[0]", "true")
        ));

class ConfigurationElement_testSerialise : public ::testing::TestWithParam<std::tuple<const char*, const char*>>
{

};

TEST_P(ConfigurationElement_testSerialise, testExpectedEquality)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    dagbase::ConfigurationElement* config = nullptr;
    {

        config = dagbase::ConfigurationElement::fromFile(lua, configStr);
        ASSERT_NE(nullptr, config);
    }
    std::string className = std::get<1>(GetParam());
    dagbase::StreamFormat* sut = nullptr;
    dagbase::MemoryBackingStore backingStore(dagbase::BackingStore::MODE_OUTPUT_BIT);
    if (className == "TextFormat")
        sut = new dagbase::TextFormat(&backingStore);
    else if (className == "BinaryFormat")
        sut = new dagbase::BinaryFormat(&backingStore);
    ASSERT_NE(nullptr, sut);
    sut->setMode(dagbase::StreamFormat::MODE_OUTPUT);
    auto ostr = new dagbase::FormatAgnosticOutputStream(sut, &backingStore);
    if (ostr->writeRef(config))
    {
        config->write(*ostr);
    }
    sut->flush();
    sut->setMode(dagbase::StreamFormat::MODE_INPUT);
    backingStore.open(dagbase::BackingStore::MODE_INPUT_BIT);

    dagbase::ConfigurationElement* configFromStream = nullptr;
    dagbase::FormatAgnosticInputStream istr(sut, &backingStore);
    dagbase::FormatAgnosticInputStream::ObjId id;
    configFromStream = istr.readRef<dagbase::ConfigurationElement>(&id, lua);
    ASSERT_NE(nullptr, configFromStream);
    EXPECT_EQ(*config, *configFromStream);
    auto funcConfig = config->findElement("foo");
    auto funcConfig2 = configFromStream->findElement("foo");
    if (funcConfig && funcConfig2)
    {
        auto func1 = funcConfig->asFunction();
        auto func2 = funcConfig2->asFunction();

        if (func1 && func2)
        {
            (*func1)(0,0);
            (*func2)(0,0);
        }
    }
    delete configFromStream;
    delete ostr;
    delete sut;
    delete config;
}

INSTANTIATE_TEST_SUITE_P(ConfigurationElement, ConfigurationElement_testSerialise, ::testing::Values(
        std::make_tuple("data/tests/ConfigurationElement/SerialisePrimitives.lua", "TextFormat"),
        std::make_tuple("data/tests/ConfigurationElement/SerialisePrimitives.lua", "BinaryFormat"),
        std::make_tuple("data/tests/ConfigurationElement/SerialiseFunction.lua", "TextFormat"),
        std::make_tuple("data/tests/ConfigurationElement/SerialiseFunction.lua", "BinaryFormat")
        ));
