//
// Created by Tony Horrobin on 19/11/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include <cstdint>
#include <cstddef>
#include <variant>
#include <vector>

namespace dagbase
{
    class Value;

    class DAGBASE_API Arguments
    {
    public:
        void append(const Value& value);

        void get(std::uint32_t index, Value* value);
    private:
        struct Index
        {
            std::variant<std::uint8_t*, std::int8_t*, std::uint16_t*, std::int16_t*, std::uint32_t*, std::int32_t*, std::uint64_t*, std::int64_t*> ptr;
        };
        using IndexArray = std::vector<Index>;
        IndexArray _indices;
        using Buffer = std::vector<std::byte>;
        Buffer _buffer;
    };
}
