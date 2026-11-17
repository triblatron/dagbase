//
// Created by Tony Horrobin on 07/11/2026.
//

#include "core/CommandSystem.h"
#include "core/Command.h"
#include "core/LuaInterface.h"

#include <gtest/gtest.h>

#include <fstream>

#include "test/TestUtils.h"

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
    bool _instant{true};
};

TEST(CommandSystem_testRegisterDuplicate, testExpectedResult)
{
    auto cmd = new TestCommand();
    dagbase::CommandSystem sut;
    auto status = sut.registerCommand(0, cmd);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status);
    status = sut.registerCommand(0, cmd);
    ASSERT_EQ(dagbase::Status::STATUS_DUPLICATE_OPCODE, status.status);
}

TEST(CommandSystem_testExecuteSuccessful, testExpectedResult)
{
    auto cmd = new TestCommand();
    dagbase::CommandSystem sut;
    auto status = sut.registerCommand(0, cmd);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    status = sut.makeItSo(0, {}, nullptr);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(dagbase::Variant(std::int64_t{1}), sut.find("numCalls"));
    status = sut.undo();
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(dagbase::Variant(std::int64_t{1}), sut.find("numUndos"));
    status = sut.undo();
    ASSERT_EQ(dagbase::Status::STATUS_EMPTY_STACK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(dagbase::Variant(std::int64_t{1}), sut.find("numUndos"));
    status = sut.redo(nullptr);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(dagbase::Variant(std::int64_t{2}), sut.find("numCalls"));
    status = sut.redo(nullptr);
    ASSERT_EQ(dagbase::Status::STATUS_EMPTY_STACK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(dagbase::Variant(std::int64_t{2}), sut.find("numCalls"));
}

TEST(CommandSystem_testExecuteUnknownOpcode, testExpectedResult)
{
    auto cmd = new TestCommand();
    dagbase::CommandSystem sut;
    auto status = sut.registerCommand(0, cmd);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    status = sut.makeItSo(1, {}, nullptr);
    ASSERT_EQ(dagbase::Status::STATUS_UNKNOWN_OPCODE, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(0, cmd->numCalls);
}

TEST(CommandSystem_testExecuteWithSideEffects, testExpectedResult)
{
    auto cmd = new TestCommand(false);
    dagbase::CommandSystem sut;
    auto status = sut.registerCommand(0, cmd);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    status = sut.makeItSo(0, {}, nullptr);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    ASSERT_EQ(dagbase::Status::WARNING_CANNOT_UNDO, status.warning) << "Got " << dagbase::Status::warningToString(status.warning);
    EXPECT_EQ(dagbase::Variant(), sut.find("numCalls"));
    status = sut.undo();
    ASSERT_EQ(dagbase::Status::STATUS_EMPTY_STACK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(dagbase::Variant(), sut.find("numUndos"));
}

TEST(CommandSystem_testExecuteLongRunning, testExpectedResult)
{
    auto cmd = new TestCommand(true, false);
    dagbase::CommandSystem sut;
    auto status = sut.registerCommand(0, cmd);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    dagbase::CommandSystem::CommandArray::Ident ident;
    status = sut.makeItSo(0, {}, nullptr, &ident);
    ASSERT_EQ(dagbase::Status::STATUS_UNKNOWN, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    status = sut.step();
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    auto actual = sut.command(ident);
    ASSERT_NE(nullptr, actual);
    EXPECT_TRUE(actual->isComplete());
    sut.free(ident);
    status = sut.makeItSo(0, {}, nullptr, &ident);
    ASSERT_EQ(dagbase::Status::STATUS_UNKNOWN, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    auto actual2 = sut.command(ident);
    ASSERT_EQ(actual, actual2);
    EXPECT_FALSE(actual2->isComplete());
    status = sut.step();
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    sut.free(ident);
}

TEST(CommandSystem_testExecuteMultipleInFlight, testExpectedResult)
{
    auto cmd = new TestCommand(true, false);
    dagbase::CommandSystem sut;
    auto status = sut.registerCommand(0, cmd);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    dagbase::CommandSystem::CommandArray::Ident ident, ident2;
    status = sut.makeItSo(0, {}, nullptr, &ident);
    ASSERT_EQ(dagbase::Status::STATUS_UNKNOWN, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    status = sut.makeItSo(0, {}, nullptr, &ident2);
    ASSERT_EQ(dagbase::Status::STATUS_UNKNOWN, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    status = sut.step();
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    auto actual = sut.command(ident);
    ASSERT_NE(nullptr, actual);
    EXPECT_TRUE(actual->isComplete());
    sut.free(ident);
    auto actual2 = sut.command(ident2);
    ASSERT_NE(nullptr, actual2);
    EXPECT_TRUE(actual2->isComplete());
    EXPECT_NE(actual,actual2);
    sut.free(ident2);
}

class CommandSystem_testLog : public ::testing::TestWithParam<std::tuple<std::uint32_t, std::pmr::vector<dagbase::Variant>, const char*>>
{

};

TEST_P(CommandSystem_testLog, testExpectedResult)
{
    auto opcode = std::get<0>(GetParam());
    auto args = std::get<1>(GetParam());
    auto log = std::get<2>(GetParam());
    dagbase::CommandSystem sut;
    std::ostringstream oss;
    sut.setLog(&oss);
    auto actualStatus = sut.log(opcode, args);
    ASSERT_EQ(dagbase::Status::STATUS_OK, actualStatus.status) << "Got " << dagbase::Status::statusCodeToString(actualStatus.status);
    auto actualLog = oss.str();
    EXPECT_EQ(log, actualLog);
}

INSTANTIATE_TEST_CASE_P(CommandSystem, CommandSystem_testLog, ::testing::Values(
    std::make_tuple(std::uint32_t{0}, std::pmr::vector<dagbase::Variant>{dagbase::Variant("GroupTyped"), dagbase::Variant("group1")}, "op(0,\"GroupTyped\",\"group1\")\n")
    ));

class CommandSystem_testPlayback : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};


TEST_P(CommandSystem_testPlayback, testExpectedResult)
{
    auto logFilename = std::get<0>(GetParam());
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    dagbase::CommandSystem sut;
    auto cmd = new TestCommand(true, true);
    sut.registerCommand(0, cmd);
    std::pmr::vector<dagbase::Variant> results;
    auto actualStatus = sut.makeItSo(0, {}, &results);
    ASSERT_EQ(dagbase::Status::STATUS_OK, actualStatus.status);
//    dagbase::Function func(lua.get(), "operation");
    sut.playback(logFilename);
    auto actual = sut.find(path);
    assertComparison(value, actual, tolerance, op, path);
}

INSTANTIATE_TEST_SUITE_P(CommandSystem, CommandSystem_testPlayback, ::testing::Values(
    std::make_tuple("data/tests/CommandSystem/TransactionLog.lua", "numCalls", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
    ));
