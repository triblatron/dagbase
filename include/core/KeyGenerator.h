//
// Created by tony on 23/03/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Types.h"

namespace dagbase
{
    class DAGBASE_API KeyGenerator
    {
    public:
        virtual ~KeyGenerator() = default;

        virtual NodeID nextNodeID() = 0;

        virtual PortID nextPortID() = 0;
    };
}
