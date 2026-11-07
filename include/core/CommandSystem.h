//
// Created by Tony Horrobin on 07/11/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Variant.h"
#include "core/Status.h"
#include "util/VectorMap.h"

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
    public:
        dagbase::Status registerCommand(std::uint32_t opcode, dagbase::Command* command);

        dagbase::Status makeItSo(std::uint32_t opcode, const std::pmr::vector<dagbase::Variant>& args, std::pmr::vector<dagbase::Variant>* results);

        dagbase::Status undo();

        dagbase::Status redo(std::pmr::vector<dagbase::Variant>* results);
    private:
        CommandMap _commands;
        CommandStack _undoStack;
        CommandStack _redoStack;
    };
}