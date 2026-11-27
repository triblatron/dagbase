//
// Created by Tony Horrobin on 07/11/2026.
//

#include "core/CommandSystem.h"
#include "core/Command.h"
#include "core/LuaInterface.h"
#include "test/TestUtils.h"
#include "util/enums.h"
#include "core/Arguments.h"

#include <gtest/gtest.h>

#include <fstream>


class TestCommand : public dagbase::Command
{
public:
    explicit TestCommand(bool canUndo=true, bool instant=true)
        :
    _instant(instant)
    {
        if (!canUndo)
            clearFlag(CMD_UNDOABLE_BIT);

        if (!instant)
            setFlag(CMD_LONG_RUNNING_BIT);
    }

    TestCommand(const TestCommand& other)
        :
    Command(other)
    {
        _instant = other._instant;
    }

    dagbase::Status makeItSo(const std::pmr::vector<dagbase::Variant>& args, std::pmr::vector<dagbase::Variant>* results) override
    {
        dagbase::Status status;

        ++numCalls;

        if (_instant || numCalls == 2)
        {
            status.status = dagbase::Status::STATUS_OK;
            markComplete();
        }

        return status;
    }

    void reset() override
    {
        Command::reset();
        numCalls = 0;
        numUndos = 0;
        ++numResets;
    }

    dagbase::Status step() override
    {
        dagbase::Status status{dagbase::Status::STATUS_OK};

        markComplete();

        return status;
    }

    dagbase::Variant find(std::string_view path) const override
    {
        dagbase::Variant result;

        result = dagbase::findEndpoint(path, "numCalls", std::int64_t{numCalls});
        if (result.has_value())
            return result;

        result = dagbase::findEndpoint(path, "numUndos", std::int64_t{numUndos});
        if (result.has_value())
            return result;

        result = dagbase::findEndpoint(path, "numResets", std::int64_t{numResets});
        if (result.has_value())
            return result;

        return {};
    }

    dagbase::Status undo() override
    {
        ++numUndos;
        return dagbase::Status{dagbase::Status::STATUS_OK};
    }

    dagbase::Status redo(std::pmr::vector<dagbase::Variant>* results) override
    {
        return makeItSo({}, results);
    }

    [[nodiscard]]TestCommand* clone() const override
    {
        return new TestCommand(*this);
    }
public:
    int numCalls{0};
    int numUndos{0};
    int numResets{0};
    bool _instant{true};
};

struct CommandSystemCase
{
    enum Command : std::uint32_t
    {
        CMD_UNKNOWN,
        CMD_REGISTER,
        CMD_EXECUTE,
        CMD_UNDO,
        CMD_REDO,
        CMD_STEP
    };

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig<Command>(config, "cmd", &parseCommand, &cmd);
        dagbase::ConfigurationElement::readConfig(config, "opcode", &opcode);
        dagbase::ConfigurationElement::readConfig(config, "canUndo", &canUndo);
        dagbase::ConfigurationElement::readConfig(config, "instant", &instant);
        dagbase::ConfigurationElement::readConfig(config, "status", &status);
        dagbase::ConfigurationElement::readConfigVector(config, "assertions", &assertions);
        dagbase::ConfigurationElement::readConfig<dagbase::ConfigurationElement::RelOp>(config, "identOp", &dagbase::ConfigurationElement::parseRelOp, &identOp);
    }

    void makeItSo(dagbase::CommandSystem& sut, dagbase::CommandSystem::CommandArray::Ident* ident)
    {
        dagbase::Status actualStatus{dagbase::Status::STATUS_UNKNOWN};
        dagbase::CommandSystem::CommandArray::Ident actualIdent;
        switch (cmd)
        {
            case CMD_REGISTER:
            {
                auto command = new TestCommand(canUndo, instant);
                actualStatus = sut.registerCommand(opcode, command);
                break;
            }
            case CMD_EXECUTE:
            {
                actualStatus = sut.makeItSo(0, {}, nullptr, &actualIdent);
                if (ident)
                {
                    switch (identOp)
                    {
                        case dagbase::ConfigurationElement::RELOP_EQ:
                            EXPECT_EQ(ident->index, actualIdent.index);
                            break;
                        case dagbase::ConfigurationElement::RELOP_NE:
                            EXPECT_NE(ident->index, actualIdent.index);
                            break;
                        case dagbase::ConfigurationElement::RELOP_UNKNOWN:
                            // NOP
                            break;
                        default:
                            FAIL() << "Unsupported identOp "  << dagbase::ConfigurationElement::relOpToString(identOp);
                            break;
                    }
                    *ident = actualIdent;
                }
                break;
            }
            case CMD_UNDO:
            {
                actualStatus = sut.undo();
                break;
            }
            case CMD_REDO:
            {
                actualStatus = sut.redo(nullptr);
                break;
            }
            case CMD_STEP:
            {
                actualStatus = sut.step();
                auto actual = sut.command(*ident);
                ASSERT_NE(nullptr, actual);
                EXPECT_TRUE(actual->isComplete());
                sut.free(*ident);
                break;
            }
            default:
                FAIL() << "Unhandled command " << commandToString(cmd);
                break;
        }
        EXPECT_EQ(status, actualStatus);
        auto actualCommand = sut.command(actualIdent);

        if (actualCommand)
        {
            for (const auto& a : assertions)
            {
                a.makeItSo(*actualCommand, commandToString(cmd));
            }
        }

    }

    static const char* commandToString(Command cmd)
    {
        switch (cmd)
        {
            ENUM_NAME(CMD_UNKNOWN)
            ENUM_NAME(CMD_REGISTER)
            ENUM_NAME(CMD_EXECUTE)
            ENUM_NAME(CMD_UNDO)
            ENUM_NAME(CMD_REDO)
            ENUM_NAME(CMD_STEP)
        }

        return "<error>";
    }

    static Command parseCommand(const char* str)
    {
        TEST_ENUM(CMD_REGISTER, str)
        TEST_ENUM(CMD_EXECUTE, str)
        TEST_ENUM(CMD_UNDO, str)
        TEST_ENUM(CMD_REDO, str)
        TEST_ENUM(CMD_STEP, str)

        return CMD_UNKNOWN;
    }

    dagbase::Status status{dagbase::Status::STATUS_UNKNOWN};
    std::uint32_t opcode{0};
    Command cmd{CMD_UNKNOWN};
    bool canUndo{true};
    bool instant{true};
    dagbase::ConfigurationElement::RelOp identOp{dagbase::ConfigurationElement::RELOP_UNKNOWN};
    using AssertionArray = std::vector<Assertion<dagbase::Command>>;
    AssertionArray assertions;
};

class CommandSystem_testScripted : public ::testing::TestWithParam<std::tuple<const char*>>
{
public:
    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfigVector(config, "cases", &_cases);
    }

    void makeItSo()
    {
        dagbase::CommandSystem::CommandArray::Ident ident;
        for (auto testCase : _cases)
        {
            testCase.makeItSo(_sut, &ident);
        }
    }
protected:
    using CaseArray = std::vector<CommandSystemCase>;
    CaseArray _cases;
    dagbase::CommandSystem _sut;
};

TEST_P(CommandSystem_testScripted, testExpectedResult)
{
    auto configFilename = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configFilename);
    ASSERT_NE(nullptr, config);
    configure(*config);
    makeItSo();
}

INSTANTIATE_TEST_SUITE_P(CommandSystem, CommandSystem_testScripted, ::testing::Values(
    std::make_tuple("data/tests/CommandSystem/Register.lua"),
    std::make_tuple("data/tests/CommandSystem/RegisterDuplicateOpcode.lua"),
    std::make_tuple("data/tests/CommandSystem/ExecuteSuccessful.lua"),
    std::make_tuple("data/tests/CommandSystem/ExecuteWithSideEffects.lua"),
    std::make_tuple("data/tests/CommandSystem/ExecuteLongRunning.lua"),
    std::make_tuple("data/tests/CommandSystem/ExecuteMultipleInFlight.lua")
    ));

class CommandSystem_testLog : public ::testing::TestWithParam<std::tuple<std::uint32_t, std::pmr::vector<dagbase::Variant>, const char*>>
{

};

TEST_P(CommandSystem_testLog, testExpectedResult)
{
    auto opcode = std::get<0>(GetParam());
    auto args = std::get<1>(GetParam());
    auto log = std::get<2>(GetParam());
    dagbase::CommandSystem sut;
    sut.registerCommand(opcode, new TestCommand(true, true));
    std::ostringstream oss;
    sut.setLog(&oss);
    std::pmr::vector<dagbase::Variant> results;
    auto actualStatus = sut.makeItSo(opcode, args, &results);
    ASSERT_EQ(dagbase::Status::STATUS_OK, actualStatus.status) << "Got " << dagbase::Status::statusCodeToString(actualStatus.status);
    auto actualLog = oss.str();
    EXPECT_EQ(log, actualLog);
}

INSTANTIATE_TEST_CASE_P(CommandSystem, CommandSystem_testLog, ::testing::Values(
    std::make_tuple(std::uint32_t{0}, std::pmr::vector<dagbase::Variant>{dagbase::Variant("GroupTyped"), dagbase::Variant("group1")}, "op(0,\"GroupTyped\",\"group1\")\n")
    ));

class CommandSystem_testPlayback : public ::testing::TestWithParam<std::tuple<const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};


TEST_P(CommandSystem_testPlayback, testExpectedResult)
{
    auto path = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    auto tolerance = std::get<2>(GetParam());
    auto op = std::get<3>(GetParam());
    dagbase::CommandSystem sut;
    auto cmd = new TestCommand(true, true);
    sut.registerCommand(0, cmd);
    std::ostringstream logStr;
    sut.setLog(&logStr);
    std::pmr::vector<dagbase::Variant> results;
    auto actualStatus = sut.makeItSo(0, {}, &results);
    ASSERT_EQ(dagbase::Status::STATUS_OK, actualStatus.status);
//    dagbase::Function func(lua.get(), "operation");
    std::istringstream playbackStr(logStr.str());
    sut.playback(playbackStr);
    auto actual = sut.find(path);
    assertComparison(value, actual, tolerance, op, path);
}

INSTANTIATE_TEST_SUITE_P(CommandSystem, CommandSystem_testPlayback, ::testing::Values(
    std::make_tuple("numCalls", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
    ));

struct ArgumentsCase
{
    enum Command : std::uint32_t
    {
        CMD_UNKNOWN,
        CMD_APPEND,
        CMD_GET
    };

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig<Command>(config, "cmd", &parseCommand, &cmd);
        dagbase::ConfigurationElement::readConfig(config, "value", &value, true);
        dagbase::ConfigurationElement::readConfig(config, "index", &index);
        dagbase::ConfigurationElement::readConfig(config, "bufferSize", &bufferSize);
        dagbase::ConfigurationElement::readConfig(config, "status", &status);
    }

    void makeItSo(dagbase::Arguments& sut)
    {
        dagbase::Status actualStatus;

        switch (cmd)
        {
            case CMD_APPEND:
            {
                actualStatus = sut.append(std::get<dagbase::Value>(value.value().value()));
                EXPECT_EQ(bufferSize, sut.bufferSize());
                break;
            }
            case CMD_GET:
            {
                actual = std::make_shared<dagbase::Value>();//new dagbase::Value();
                actualStatus = sut.get(index, actual.get());

                EXPECT_EQ(value, dagbase::Variant(*actual));

                break;
            }
            default:
                FAIL() << "Unhandled command " << commandToString(cmd);
        }

        EXPECT_EQ(status, actualStatus);
    }

    static const char* commandToString(Command value)
    {
        switch (value)
        {
            ENUM_NAME(CMD_UNKNOWN)
            ENUM_NAME(CMD_APPEND)
            ENUM_NAME(CMD_GET)
        }

        return "<error>";
    }

    static Command parseCommand(const char* str)
    {
        TEST_ENUM(CMD_APPEND, str)
        TEST_ENUM(CMD_GET, str)

        return CMD_UNKNOWN;
    }
    Command cmd{CMD_UNKNOWN};
    dagbase::Variant value;
    std::uint32_t index{0};
    std::uint32_t bufferSize{0};
    dagbase::Status status{dagbase::Status::STATUS_OK};
    std::shared_ptr<dagbase::Value> actual;
};

class Arguments_testAppend : public ::testing::TestWithParam<std::tuple<const char*>>
{
public:
    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfigVector(config, "cases", &_cases);
        std::uint32_t size{0};
        dagbase::ConfigurationElement::readConfig(config, "size", &size);
        _sut = new dagbase::Arguments(size);
    }

    void makeItSo()
    {
        for (auto& testCase : _cases)
        {
            testCase.makeItSo(*_sut);
        }
    }
protected:
    using CaseArray = std::vector<ArgumentsCase>;
    CaseArray _cases;
    dagbase::Arguments* _sut{nullptr};
};

TEST_P(Arguments_testAppend, testExpectedResult)
{
    auto configFilename = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configFilename);
    ASSERT_NE(nullptr, config);
    configure(*config);
    makeItSo();
}

INSTANTIATE_TEST_SUITE_P(Arguments, Arguments_testAppend, ::testing::Values(
    std::make_tuple("data/tests/Arguments/UInt8.lua"),
    std::make_tuple("data/tests/Arguments/Int64.lua"),
    std::make_tuple("data/tests/Arguments/Mixture.lua"),
    std::make_tuple("data/tests/Arguments/MixturePreallocated.lua"),
    std::make_tuple("data/tests/Arguments/Compound.lua"),
    std::make_tuple("data/tests/Arguments/String.lua")
    ));

TEST(Arguments, testGetEmptyArguments)
{
    dagbase::Arguments sut;
    dagbase::Value value;
    auto actualStatus = sut.get(0,&value);
    EXPECT_EQ(dagbase::Status{dagbase::Status::STATUS_ARGUMENT_OUT_OF_RANGE}, actualStatus);
}

TEST(Arguments, testGetInvalidArgument)
{
    dagbase::Arguments sut;
    dagbase::Value value;
    value = std::int64_t{1};
    auto actualStatus =  sut.append(value);
    ASSERT_EQ(dagbase::Status{dagbase::Status::STATUS_OK}, actualStatus);
    actualStatus = sut.get(0,nullptr);
    EXPECT_EQ(dagbase::Status(dagbase::Status::STATUS_ARGUMENT_NULL), actualStatus);
}
