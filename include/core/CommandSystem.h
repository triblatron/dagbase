//
// Created by Tony Horrobin on 07/11/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Variant.h"
#include "core/Status.h"
#include "util/VectorMap.h"
#include "util/SlotArray.h"
#include <cstdint>
#include <vector>
#include <stack>

namespace dagbase
{
    class Command;

    class DAGBASE_API CommandSystem
    {
    public:
        using CommandMap = dagbase::VectorMap<std::uint32_t, Command*>;
        using CommandStack = std::stack<Command*>;
        using CommandArray = SlotArray<Command>;
    public:
        ~CommandSystem();

        dagbase::Status registerCommand(std::uint32_t opcode, dagbase::Command* command);

        dagbase::Status makeItSo(std::uint32_t opcode, const std::pmr::vector<dagbase::Variant>& args, std::pmr::vector<dagbase::Variant>* results, CommandArray::Ident* ident=nullptr);

        dagbase::Status step();

        Command* command(const CommandArray::Ident& ident)
        {
            auto item = _inFlightCommands.tryGet(ident);
            return item;
        }

        void free(const CommandArray::Ident& ident)
        {
            _inFlightCommands.free(&_inFlightCommands.get(ident));
        }

        dagbase::Status undo();

        dagbase::Status redo(std::pmr::vector<dagbase::Variant>* results);
    private:
        CommandMap _commands;
        CommandStack _undoStack;
        CommandStack _redoStack;
        CommandArray _inFlightCommands;
    };
}