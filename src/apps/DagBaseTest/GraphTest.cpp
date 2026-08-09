//
// Created by Tony Horrobin on 08/08/2026.
//

#include "core/SignalPathTable.h"
#include "core/ConfigurationElement.h"
#include "util/enums.h"

#include <gtest/gtest.h>

#include "core/SignalPath.h"
#include "core/Graph.h"
#include "test/TestUtils.h"

using SignalPathAssertion = Assertion<dagbase::SignalPathTable, dagbase::SignalPathTable::FindResult>;

struct SignalPathScriptItem
{
    enum Command
    {
        COMMAND_UNKNOWN,
        COMMAND_ADD,
        COMMAND_FIND_FROM,
        COMMAND_FIND_TO
    };

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig(config, "status", &status);
        dagbase::ConfigurationElement::readConfig<Command>(config, "command", &parseCommand, &cmd);
        dagbase::ConfigurationElement::readConfig(config, "from", &from);
        dagbase::ConfigurationElement::readConfig(config, "to", &to);
        dagbase::ConfigurationElement::readConfigVector(config, "assertions", &assertions);
    }

    void makeItSo(dagbase::SignalPathTable& sut, dagbase::KeyGenerator& keyGen, const std::string& caseName)
    {
        dagbase::Status actualStatus;
        dagbase::SignalPathTable::FindResult result;

        switch (cmd)
        {
            case COMMAND_ADD:
            {
                auto* signalPath = new dagbase::SignalPath(keyGen, from, to);

                actualStatus = sut.add(signalPath);

                break;
            }
            case COMMAND_FIND_FROM:
            {
                sut.findBySource(from, &result);

                break;
            }
            case COMMAND_FIND_TO:
            {
                sut.findByDest(to, &result);

                break;
            }
            default:
                FAIL() << "Handling unknown command";
                break;
        }

        ASSERT_EQ(status.status, actualStatus.status)  << caseName << ':' << commandToString(cmd) << ":Expected a status of " << dagbase::Status::statusCodeToString(status.status) << ", got " << dagbase::Status::statusCodeToString(actualStatus.status);
        ASSERT_EQ(status.resultType, actualStatus.resultType) << caseName << ':' << commandToString(cmd) << ":Expected a resultType of " << dagbase::Status::resultTypeToString(status.resultType) << ", got " << dagbase::Status::resultTypeToString(actualStatus.resultType);
        ASSERT_EQ(status.result, actualStatus.result) << caseName << ':' << commandToString(cmd);

        for (const auto& a : assertions)
        {
            a.makeItSo(sut, result, caseName + ':' + commandToString(cmd));
        }
    }

    static const char* commandToString(Command value)
    {
        switch (value)
        {
            ENUM_NAME(COMMAND_UNKNOWN)
            ENUM_NAME(COMMAND_ADD)
            ENUM_NAME(COMMAND_FIND_FROM)
            ENUM_NAME(COMMAND_FIND_TO)
        }

        return "<error>";
    }

    static Command parseCommand(const char* str)
    {
        TEST_ENUM(COMMAND_ADD, str);
        TEST_ENUM(COMMAND_FIND_FROM, str);
        TEST_ENUM(COMMAND_FIND_TO, str);

        return COMMAND_UNKNOWN;
    }

    Command cmd{COMMAND_UNKNOWN};
    dagbase::PortID from{dagbase::SignalPathID::INVALID_ID};
    dagbase::PortID to{dagbase::SignalPathID::INVALID_ID};
    dagbase::Status status{dagbase::Status::STATUS_UNKNOWN};
    using AssertionArray = std::vector<SignalPathAssertion>;
    AssertionArray assertions;
};

class SignalPathScript
{
public:
    using ItemArray = std::vector<SignalPathScriptItem>;

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfigVector(config, "items", &_items);
    }

    void setUp()
    {
        _sut = new dagbase::SignalPathTable();
        _keyGen = new dagbase::Graph();
    }

    void tearDown()
    {
        delete _keyGen;
        delete _sut;
    }

    void makeItSo()
    {
        setUp();
        // Original pattern with no explicit cases, just an implicit one with no substitutions
        for (auto item : _items)
        {
            item.makeItSo(*_sut, *_keyGen, "<implicit>");
        }
        tearDown();
    }
private:
    dagbase::SignalPathTable* _sut{nullptr};
    dagbase::Graph* _keyGen{nullptr};
    ItemArray _items;
};

class SignalPathTable_testScripted : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

TEST_P(SignalPathTable_testScripted, testExpectedValue)
{
    auto filename = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, filename);
    ASSERT_NE(nullptr, config);
    SignalPathScript script;
    script.configure(*config);
    script.makeItSo();
}

INSTANTIATE_TEST_SUITE_P(SignalPathTable, SignalPathTable_testScripted, ::testing::Values(
    std::make_tuple("data/tests/SignalPathTable/InsertInvalid.lua"),
    std::make_tuple("data/tests/SignalPathTable/InsertValid.lua"),
    std::make_tuple("data/tests/SignalPathTable/Query.lua")
    ));