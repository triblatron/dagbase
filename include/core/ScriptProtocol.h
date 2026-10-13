//
// Created by Tony Horrobin on 12/10/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Variant.h"

#include <memory_resource>

namespace dagbase
{
    class DAGBASE_API ScriptProtocol
    {
    public:
        virtual ~ScriptProtocol() = default;

        virtual void request(std::uint32_t opCode, const std::pmr::vector<Variant>& args) = 0;
    };
}
