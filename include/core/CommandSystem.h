//
// Created by Tony Horrobin on 07/11/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Variant.h"

#include <cstdint>
#include <vector>
#include <stack>

namespace dagbase
{
    class Command;

    class DAGBASE_API CommandSystem
    {
    public:
        using CommandArray = std::vector<Command*>;
        using CommandStack = std::stack<Command*>;
    public:
        void registerCommand(dagbase::Command* command);

        void makeItSo(std::uint32_t opcode, const std::pmr::vector<dagbase::Variant>& args, std::pmr::vector<dagbase::Variant>* results);

        void undo();

        void redo();
    private:
        CommandArray _commands;
        CommandStack _undoStack;
        CommandStack _redoStack;
    };
}