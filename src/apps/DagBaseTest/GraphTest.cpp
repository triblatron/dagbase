//
// Created by Tony Horrobin on 08/08/2026.
//

#include "core/SignalPathTable.h"
#include "core/ConfigurationElement.h"
#include "util/enums.h"

#include <gtest/gtest.h>

#include "core/SignalPath.h"
#include "core/Graph.h"
#include "core/NodesPortsTable.h"
#include "test/TestUtils.h"

using SignalPathAssertion = Assertion<dagbase::SignalPathTable, dagbase::SignalPathTable::FindResultFrom>;

struct SignalPathScriptItem
{
    enum Command
    {
        COMMAND_UNKNOWN,
        COMMAND_ADD,
        COMMAND_FIND_FROM,
        COMMAND_FIND_TO,
        COMMAND_FIND_FULL,
        COMMAND_FIND_ID,
        COMMAND_REMOVE
    };

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig(config, "status", &status);
        dagbase::ConfigurationElement::readConfig<Command>(config, "command", &parseCommand, &cmd);
        dagbase::ConfigurationElement::readConfig(config, "from", &from);
        dagbase::ConfigurationElement::readConfig(config, "to", &to);
        dagbase::ConfigurationElement::readConfig(config, "id", &id);
        dagbase::ConfigurationElement::readConfig<dagbase::ConfigurationElement::RelOp>(config, "op", &dagbase::ConfigurationElement::parseRelOp, &op);
        dagbase::ConfigurationElement::readConfig(config, "removed", &removed);
        dagbase::ConfigurationElement::readConfigVector(config, "assertions", &assertions);
    }

    void makeItSo(dagbase::SignalPathTable& sut, dagbase::KeyGenerator& keyGen, const std::string& caseName)
    {
        dagbase::Status actualStatus;
        dagbase::SignalPathTable::FindResultFrom result;

        switch (cmd)
        {
            case COMMAND_ADD:
            {
                auto* signalPath = new dagbase::SignalPath(keyGen, from, to);
                if (removed)
                {
                    signalPath->markRemoved();
                }
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
            case COMMAND_FIND_FULL:
            {
                sut.findFull(from, to, &result);

                break;
            }
            case COMMAND_FIND_ID:
            {
                dagbase::SignalPath* retval = sut.findByID(id);
                switch (op)
                {
                    case dagbase::ConfigurationElement::RELOP_NOT_NULL:
                        ASSERT_NE(nullptr, retval);
                        break;
                    case dagbase::ConfigurationElement::RELOP_NULL:
                        ASSERT_EQ(nullptr, retval);
                        break;
                    default:
                        FAIL() << "Unexpected op " << dagbase::ConfigurationElement::relOpToString(op);
                        break;
                }
                break;
            }
            case COMMAND_REMOVE:
            {
                actualStatus = sut.remove(id);
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
            ENUM_NAME(COMMAND_FIND_FULL)
            ENUM_NAME(COMMAND_FIND_ID)
            ENUM_NAME(COMMAND_REMOVE)
        }

        return "<error>";
    }

    static Command parseCommand(const char* str)
    {
        TEST_ENUM(COMMAND_ADD, str);
        TEST_ENUM(COMMAND_FIND_FROM, str);
        TEST_ENUM(COMMAND_FIND_TO, str);
        TEST_ENUM(COMMAND_FIND_FULL, str);
        TEST_ENUM(COMMAND_FIND_ID, str);
        TEST_ENUM(COMMAND_REMOVE, str);

        return COMMAND_UNKNOWN;
    }

    Command cmd{COMMAND_UNKNOWN};
    dagbase::SignalPathID id{dagbase::SignalPathID::INVALID_ID};
    dagbase::PortID from{dagbase::SignalPathID::INVALID_ID};
    dagbase::PortID to{dagbase::SignalPathID::INVALID_ID};
    dagbase::Status status{dagbase::Status::STATUS_UNKNOWN};
    dagbase::ConfigurationElement::RelOp op{dagbase::ConfigurationElement::RELOP_UNKNOWN};
    bool removed{false};
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
    std::make_tuple("data/tests/SignalPathTable/Query.lua"),
    std::make_tuple("data/tests/SignalPathTable/QueryRemoved.lua")
    ));

using NodesPortsAssertion = Assertion<dagbase::NodesPortsTable, dagbase::NodesPortsTable::FindResult>;

struct NodesPortsScriptItem
{
    enum Command
    {
        COMMAND_UNKNOWN,
        COMMAND_ADD_PORT,
        COMMAND_QUERY_PORTS,
        COMMAND_REMOVE_PORT
    };

    void configure(dagbase::ConfigurationElement& config);

    void makeItSo(dagbase::NodesPortsTable& sut, const std::string& caseName);

    static const char* commandToString(Command value);

    static Command parseCommand(const char* str);

    Command cmd{COMMAND_UNKNOWN};
    dagbase::NodeID nodeID{dagbase::NodeID::INVALID_ID};
    dagbase::PortID portID{dagbase::PortID::INVALID_ID};
    using AssertionArray = std::vector<NodesPortsAssertion>;
    AssertionArray assertions;
};

void NodesPortsScriptItem::configure(dagbase::ConfigurationElement &config)
{
    dagbase::ConfigurationElement::readConfig<Command>(config, "cmd", &parseCommand, &cmd);
    dagbase::ConfigurationElement::readConfig(config, "nodeID", &nodeID);
    dagbase::ConfigurationElement::readConfig(config, "portID", &portID);
    dagbase::ConfigurationElement::readConfigVector(config, "assertions", &assertions);
}

void NodesPortsScriptItem::makeItSo(dagbase::NodesPortsTable &sut, const std::string& caseName)
{
    dagbase::NodesPortsTable::FindResult result(sut);

    switch (cmd)
    {
        case COMMAND_ADD_PORT:
        {
            sut.addPort(nodeID, portID);

            break;
        }
        case COMMAND_REMOVE_PORT:
        {
            break;
        }
        case COMMAND_QUERY_PORTS:
        {
            sut.portsForNode(nodeID, &result);

            break;
        }
        default:
            FAIL() << "Got into unknown command";

            break;
    }

    for (const auto& a : assertions)
    {
        a.makeItSo(sut, result, caseName + ':' + commandToString(cmd));
    }
}

const char * NodesPortsScriptItem::commandToString(Command value)
{
    switch (value)
    {
        ENUM_NAME(COMMAND_UNKNOWN)
        ENUM_NAME(COMMAND_ADD_PORT)
        ENUM_NAME(COMMAND_REMOVE_PORT)
        ENUM_NAME(COMMAND_QUERY_PORTS)
    }

    return "<error>";
}

NodesPortsScriptItem::Command NodesPortsScriptItem::parseCommand(const char *str)
{
    TEST_ENUM(COMMAND_UNKNOWN, str);
    TEST_ENUM(COMMAND_ADD_PORT, str);
    TEST_ENUM(COMMAND_REMOVE_PORT, str);
    TEST_ENUM(COMMAND_QUERY_PORTS, str);

    return COMMAND_UNKNOWN;
}

class NodesPortsTableScript
{
public:
    using ItemArray = std::vector<NodesPortsScriptItem>;

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfigVector(config, "items", &_items);
    }

    void setUp()
    {
        _sut = new dagbase::NodesPortsTable();
    }

    void tearDown()
    {
        delete _sut;
    }

    void makeItSo()
    {
        setUp();
        // Original pattern with no explicit cases, just an implicit one with no substitutions
        for (auto item : _items)
        {
            item.makeItSo(*_sut, "<implicit>");
        }
        tearDown();
    }
private:
    dagbase::NodesPortsTable* _sut{nullptr};
    ItemArray _items;
};


class NodesPortsTable_testScripted : public ::testing::TestWithParam<std::tuple<const char*>>
{
protected:
    NodesPortsTableScript _script;
};

TEST_P(NodesPortsTable_testScripted, testExpectedValue)
{
    auto filename = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, filename);
    ASSERT_NE(nullptr, config);
    _script.configure(*config);
    _script.makeItSo();
}

INSTANTIATE_TEST_SUITE_P(NodesPortsTable, NodesPortsTable_testScripted, ::testing::Values(
    std::make_tuple("data/tests/NodesPortsTable/Add.lua")
    ));
