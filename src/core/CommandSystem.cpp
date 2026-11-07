//
// Created by Tony Horrobin on 07/11/2026.
//

#include "config/config.h"

#include "core/CommandSystem.h"

namespace dagbase
{
    void CommandSystem::registerCommand(dagbase::Command *command)
    {
    }

    void CommandSystem::makeItSo(std::uint32_t opcode, const std::pmr::vector<dagbase::Variant> &args,
        std::pmr::vector<dagbase::Variant> *results)
    {
    }

    void CommandSystem::undo()
    {
    }

    void CommandSystem::redo()
    {
    }
}
