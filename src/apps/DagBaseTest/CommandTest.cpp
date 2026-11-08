//
// Created by Tony Horrobin on 07/11/2026.
//

#include "core/CommandSystem.h"
#include "core/Command.h"

#include <gtest/gtest.h>

class TestCommand : public dagbase::Command
{
public:
    explicit TestCommand(bool canUndo=true, bool instant=true)
        :
    _instant(instant)
    {
        if (!canUndo)
            clearFlag(CMD_UNDOABLE_BIT);
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
    delete cmd;
}

TEST(CommandSystem_testExecuteSuccessful, testExpectedResult)
{
    auto cmd = new TestCommand();
    dagbase::CommandSystem sut;
    auto status = sut.registerCommand(0, cmd);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    status = sut.makeItSo(0, {}, nullptr);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(1, cmd->numCalls);
    status = sut.undo();
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(1, cmd->numUndos);
    status = sut.undo();
    ASSERT_EQ(dagbase::Status::STATUS_EMPTY_STACK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(1, cmd->numUndos);
    status = sut.redo(nullptr);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(2, cmd->numCalls);
    status = sut.redo(nullptr);
    ASSERT_EQ(dagbase::Status::STATUS_EMPTY_STACK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(2, cmd->numCalls);
    delete cmd;
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
    delete cmd;
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
    EXPECT_EQ(1, cmd->numCalls);
    status = sut.undo();
    ASSERT_EQ(dagbase::Status::STATUS_EMPTY_STACK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    EXPECT_EQ(0, cmd->numUndos);
    delete cmd;
}

TEST(CommandSystem_testExecuteLongRunning, testExpectedResult)
{
    auto cmd = new TestCommand(true, false);
    dagbase::CommandSystem sut;
    auto status = sut.registerCommand(0, cmd);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    status = sut.makeItSo(0, {}, nullptr);
    ASSERT_EQ(dagbase::Status::STATUS_UNKNOWN, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    ASSERT_FALSE(cmd->isComplete());
    status = sut.makeItSo(0, {}, nullptr);
    ASSERT_EQ(dagbase::Status::STATUS_OK, status.status) << "Got " << dagbase::Status::statusCodeToString(status.status);
    ASSERT_TRUE(cmd->isComplete());
}
