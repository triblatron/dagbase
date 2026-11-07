//
// Created by Tony Horrobin on 07/11/2026.
//

#include "config/config.h"

#include "core/CommandSystem.h"
#include "core/Command.h"

namespace dagbase
{
    dagbase::Status CommandSystem::registerCommand(std::uint32_t opcode, dagbase::Command *command)
    {
        Status status{Status::STATUS_UNKNOWN};

        if (auto it=_commands.find(opcode); it==_commands.end())
        {
            _commands.emplace(opcode, command);
            status.status = Status::STATUS_OK;
        }
        else
        {
            status.status = Status::STATUS_DUPLICATE_OPCODE;
        }

        return status;
    }

    dagbase::Status CommandSystem::makeItSo(std::uint32_t opcode, const std::pmr::vector<dagbase::Variant> &args,
        std::pmr::vector<dagbase::Variant> *results)
    {
        Status status{Status::STATUS_UNKNOWN};

        if (auto it=_commands.find(opcode); it!=_commands.end())
        {
            status = it->second->makeItSo(args, results);
            _undoStack.push(it->second);
        }
        else
        {
            status.status = Status::STATUS_UNKNOWN_OPCODE;
        }

        return status;
    }

    dagbase::Status CommandSystem::undo()
    {
        Status status{Status::STATUS_UNKNOWN};

        if (!_undoStack.empty())
        {
            auto cmd = _undoStack.top();

            status = cmd->undo();
            _redoStack.push(cmd);
            _undoStack.pop();
        }
        else
        {
            status.status = Status::STATUS_EMPTY_STACK;
        }
        return status;
    }

    dagbase::Status CommandSystem::redo(std::pmr::vector<dagbase::Variant>* results)
    {
        Status status{Status::STATUS_UNKNOWN};

        if (!_redoStack.empty())
        {
            auto cmd = _redoStack.top();
            status = cmd->redo(results);
            _redoStack.pop();
        }
        else
        {
            status.status = Status::STATUS_EMPTY_STACK;
        }

        return status;
    }
}
