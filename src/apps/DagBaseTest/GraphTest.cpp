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
#include "io/StreamFactory.h"
#include "io/Stream.h"
#include "io/InputStream.h"
#include "test/TestUtils.h"

#include <algorithm>

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
