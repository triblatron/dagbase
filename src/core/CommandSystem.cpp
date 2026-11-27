//
// Created by Tony Horrobin on 07/11/2026.
//

#include "config/config.h"

#include "core/CommandSystem.h"
#include "core/Command.h"

namespace dagbase
{
    static int operation(lua_State* L)
    {
        if (lua_gettop(L)<1)
        {
            lua_pushfstring(L, "Expected 1 arguments, got %d", lua_gettop(L));
            lua_error(L);
            return -1;
        }
        std::uint32_t opcode = lua_tointeger(L,1);
        dagbase::Lua lua(L,false);
        std::pmr::vector<dagbase::Variant> args;
        for (int i=2; i<=lua_gettop(L); ++i)
        {
            dagbase::Variant arg;
            arg.fromLua(lua, i);
            args.emplace_back(arg);
        }
        lua_getglobal(L, "cmdsys");
        auto cmdSys = static_cast<dagbase::CommandSystem*>(lua_touserdata(L, -1));
        cmdSys->makeItSo(opcode, args, nullptr);
        return 0;
    }

    CommandSystem::CommandSystem()
    {
        lua_register(_lua.get(), "op", &operation);
        lua_pushlightuserdata(_lua.get(), this);
        lua_setglobal(_lua.get(), "cmdsys");

    }

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
            log(opcode, args);
            if (cmd->isLongRunning())
            {
                auto item = _inFlightCommands.alloc(cmd);
                cmd = item->item;

                if (ident)
                    *ident = _inFlightCommands.id(item);
            }
            else
            {
                cmd = cmd->clone();
                status = cmd->makeItSo(args, results);
                CommandStack emptyRedo;
                _redoStack.swap(emptyRedo);
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
            _undoStack.push(cmd);
        }
        else
        {
            status.status = Status::STATUS_EMPTY_STACK;
        }

        return status;
    }

    dagbase::Status CommandSystem::log(std::uint32_t opcode, const std::pmr::vector<dagbase::Variant> &args)
    {
        if (_logStr && (*_logStr))
        {
            (*_logStr) << "op(";
            (*_logStr) << opcode;
            if (!args.empty())
            {
                (*_logStr) << ',';
                for (std::size_t i = 0; i < args.size(); ++i)
                {
                    if (args[i].index() == dagbase::Variant::TYPE_STRING)
                        (*_logStr) << '\"';
                    (*_logStr) << args[i].toString();
                    if (args[i].index() == dagbase::Variant::TYPE_STRING)
                        (*_logStr) << '\"';
                    if (i<args.size()-1)
                        (*_logStr) << ',';
                }
            }
            (*_logStr) << ")\n";
            _logStr->flush();
        }
        return dagbase::Status{Status::STATUS_OK};
    }

    dagbase::Status CommandSystem::playback(std::istream& logStr)
    {
        std::string log;

        logStr >> log;

        _lua.eval(log.c_str());

        if (!_lua.ok())
        {
            return dagbase::Status{Status::STATUS_SYNTAX_ERROR};
        }

        return dagbase::Status{dagbase::Status::STATUS_OK};
    }

    dagbase::Variant CommandSystem::find(std::string_view path) const
    {
        dagbase::Variant retval;

        if (!_undoStack.empty())
        {
            retval = _undoStack.top()->find(path);
            if (retval.has_value())
                return retval;
        }

        if (!_redoStack.empty())
        {
            retval = _redoStack.top()->find(path);
            if (retval.has_value())
                return retval;
        }


        return {};
    }
}
