//
// Created by Tony Horrobin on 08/11/2026.
//

#include "core/Types.h"
#include "test/TestUtils.h"
#include "core/LuaInterface.h"
#include "util/SlotArray.h"
#include "util/enums.h"

#include <gtest/gtest.h>


struct SlotArrayElement
{
    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig(config, "i", &i);
        initial = i;
    }

    bool isSameType(const SlotArrayElement& other) const
    {
        return true;
    }

    SlotArrayElement* clone()
    {
        return new SlotArrayElement(*this);
    }

    void reset()
    {
        i = initial;
    }

    int i{0};
    int initial{0};
};

struct SlotArrayScriptItem
{
    enum Command : std::uint32_t
    {
        COMMAND_UNKNOWN,
        COMMAND_ALLOC,
        COMMAND_FREE,
        COMMAND_TRY_GET,
        COMMAND_IS_VALID
    };

    Command cmd{COMMAND_UNKNOWN};

    using Sut = dagbase::SlotArray<SlotArrayElement>;
    bool exists{false};
    SlotArrayElement* element{nullptr};
    Sut::Ident id{0};
    using Assertions = std::vector<Assertion<Sut>>;
    Assertions assertions;

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig<Command>(config, "cmd", &parseCommand, &cmd);
        element = new SlotArrayElement();
        dagbase::ConfigurationElement::readConfig(config, "element", element);
        dagbase::ConfigurationElement::readConfig(config, "id", &id);
        dagbase::ConfigurationElement::readConfig(config, "exists", &exists);
        dagbase::ConfigurationElement::readConfigVector(config, "assertions", &assertions);
    }

    void makeItSo(Sut& sut)
    {
        switch (cmd)
        {
            case COMMAND_ALLOC:
            {
                auto item = sut.alloc(element);
                auto actual = sut.id(item);
                ASSERT_EQ(id, actual);

                break;
            }
            case COMMAND_FREE:
            {
                sut.free(&sut.get(id));
                ASSERT_EQ(nullptr, sut.tryGet(id));

                break;
            }
            case COMMAND_TRY_GET:
            {
                auto actual = sut.tryGet(id);
                ASSERT_EQ(exists, actual!=nullptr);
                if (exists)
                {
                    ASSERT_EQ(element->i, actual->i);
                }
                break;
            }
            case COMMAND_IS_VALID:
            {
                auto actual = sut.isValid(id);
                ASSERT_EQ(exists, actual);

                break;
            }
            default:
                FAIL() << "Got into unhandled command " << commandToString(cmd);

                break;
        }

        for (auto& a : assertions)
            a.makeItSo(sut, commandToString(cmd));
    }

    static const char* commandToString(Command value)
    {
        switch (value)
        {
            ENUM_NAME(COMMAND_UNKNOWN)
            ENUM_NAME(COMMAND_ALLOC)
            ENUM_NAME(COMMAND_FREE)
            ENUM_NAME(COMMAND_TRY_GET)
            ENUM_NAME(COMMAND_IS_VALID)
        }

        return "<error>";
    }

    static Command parseCommand(const char* str)
    {
        TEST_ENUM(COMMAND_UNKNOWN, str);
        TEST_ENUM(COMMAND_ALLOC, str);
        TEST_ENUM(COMMAND_FREE, str);
        TEST_ENUM(COMMAND_TRY_GET, str);
        TEST_ENUM(COMMAND_IS_VALID, str);

        return COMMAND_UNKNOWN;
    }
};

struct SlotArrayScript
{
    using Array = std::vector<SlotArrayScriptItem>;
    Array items;

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfigVector(config, "items", &items);
    }

    void makeItSo(SlotArrayScriptItem::Sut& sut)
    {
        for (auto item : items)
        {
            item.makeItSo(sut);
        }
    }
};

class SlotArray_testScripted : public ::testing::TestWithParam<std::tuple<const char*>>
{
protected:
    void SetUp() override
    {
        _sut = new SlotArrayScriptItem::Sut(10);
    }

    void TearDown() override
    {
        delete _sut;
    }

    SlotArrayScript _script;
    SlotArrayScriptItem::Sut* _sut{nullptr};
};

TEST_P(SlotArray_testScripted, testExpectedValue)
{
    auto filename = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, filename);
    ASSERT_NE(nullptr, config);
    _script.configure(*config);
    _script.makeItSo(*_sut);
    delete config;
}

INSTANTIATE_TEST_SUITE_P(SlotArray, SlotArray_testScripted, ::testing::Values(
    std::make_tuple("data/tests/SlotArray/AllocAndFree.lua")
    ));
