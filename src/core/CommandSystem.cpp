//
// Created by Tony Horrobin on 07/11/2026.
//

#include "config/config.h"

#include "core/CommandSystem.h"
#include "core/Command.h"

namespace dagbase
{
    CommandSystem::~CommandSystem()
    {
        for (auto p : _commands)
        {
            delete p.second;
        }
    }

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
        std::pmr::vector<dagbase::Variant> *results, CommandArray::Ident* ident)
    {
        Status status{Status::STATUS_UNKNOWN};

        if (auto it=_commands.find(opcode); it!=_commands.end())
        {
            auto cmd = it->second;
            if (cmd->isLongRunning())
            {
                auto item = _inFlightCommands.alloc(cmd);
                cmd = item->item;

                if (ident)
                    *ident = _inFlightCommands.id(item);
            }
            else
            {
                status = cmd->makeItSo(args, results);
                if (cmd->isUndoable())
                {
                    _undoStack.push(cmd);
                }
                else
                {
                    status.warning = Status::WARNING_CANNOT_UNDO;
                    CommandStack empty;
                    _undoStack.swap(empty);
                }
            }
        }
        else
        {
            status.status = Status::STATUS_UNKNOWN_OPCODE;
        }

        return status;
    }

    dagbase::Status CommandSystem::step()
    {
        Status status{Status::STATUS_OK};

        // Step long-running commands
        for (auto item : _inFlightCommands)
        {
            status = item.item->step();
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
