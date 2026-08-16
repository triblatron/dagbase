//
// Created by Tony Horrobin on 16/08/2026.
//

#include <gtest/gtest.h>

#include "util/SlotMap.h"
#include "core/ConfigurationElement.h"
#include "core/LuaInterface.h"
#include "test/TestUtils.h"
#include "util/enums.h"

struct SlotMapScriptItem
{
    enum Command : std::uint32_t
    {
        COMMAND_UNKNOWN,
        COMMAND_ALLOC,
        COMMAND_FREE,
        COMMAND_TRY_GET
    };

    Command cmd{COMMAND_UNKNOWN};

    using Sut = dagbase::SlotMap<int>;
    Sut::Ident id;
    bool exists{false};
    using Assertions = std::vector<Assertion<dagbase::SlotMap<int>>>;
    Assertions assertions;

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig<Command>(config, "cmd", &parseCommand, &cmd);
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
                auto& item = sut.alloc();
                auto actual = sut.id(item);
                ASSERT_EQ(id, actual);

                break;
            }
            case COMMAND_FREE:
            {
                sut.free(sut.get(id));
                ASSERT_EQ(nullptr, sut.tryGet(id));

                break;
            }
            case COMMAND_TRY_GET:
            {
                auto actual = sut.tryGet(id);
                ASSERT_EQ(exists, actual!=nullptr);
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
        }

        return "<error>";
    }

    static Command parseCommand(const char* str)
    {
        TEST_ENUM(COMMAND_UNKNOWN, str);
        TEST_ENUM(COMMAND_ALLOC, str);
        TEST_ENUM(COMMAND_FREE, str);
        TEST_ENUM(COMMAND_TRY_GET, str);

        return COMMAND_UNKNOWN;
    }
};

struct SlotMapScript
{
    using Array = std::vector<SlotMapScriptItem>;
    Array items;

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfigVector(config, "items", &items);
    }

    void makeItSo(SlotMapScriptItem::Sut& sut)
    {
        for (auto item : items)
        {
            item.makeItSo(sut);
        }
    }
};

class SlotMap_testScripted : public ::testing::TestWithParam<std::tuple<const char*>>
{
protected:
    void SetUp() override
    {
        _sut = new SlotMapScriptItem::Sut(10);
    }

    void TearDown() override
    {
        delete _sut;
    }

    SlotMapScript _script;
    SlotMapScriptItem::Sut* _sut{nullptr};
};

TEST_P(SlotMap_testScripted, testExpectedValue)
{
    auto filename = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, filename);
    ASSERT_NE(nullptr, config);
    _script.configure(*config);
    _script.makeItSo(*_sut);
    delete config;
}

INSTANTIATE_TEST_SUITE_P(SlotMap, SlotMap_testScripted, ::testing::Values(
    std::make_tuple("data/tests/SlotMap/AllocAndFree.lua")
    ));
