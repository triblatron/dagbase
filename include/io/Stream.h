//
// Created by tony on 09/03/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include <cstdint>
#include <cstdlib>

namespace dagbase
{
    class DAGBASE_API Stream
    {
    public:
        typedef std::uint8_t value_type;
        typedef std::uint32_t ObjId;
        typedef void* Ref;
    public:
        virtual ~Stream() = default;
    };
}
