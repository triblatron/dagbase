//
// Created by Tony Horrobin on 19/11/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Value.h"

#include <cstdint>
#include <cstddef>
#include <variant>
#include <vector>

namespace dagbase
{
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

        template<typename T>
        void insert(const Value& value, void*& ptr, std::size_t& available)
        {
            std::align(alignof(T), sizeof(T), ptr, available);
            new (ptr) T(std::get<T>(value.value()));
            Index entry;
            entry.ptr = static_cast<T*>(ptr);
            _indices.emplace_back(entry);
        }
    };
}
