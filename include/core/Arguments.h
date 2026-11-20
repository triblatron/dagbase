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
        explicit Arguments(std::size_t n);

        void append(const Value& value);

        void get(std::uint32_t index, Value* value);

        std::size_t size() const
        {
            return _indices.size();
        }

        std::size_t bufferSize() const
        {
            return _buffer.size();
        }
    private:
        struct Index
        {
            std::ptrdiff_t index;
            dagbase::Value::Type type{dagbase::Value::TYPE_UNKNOWN};
            //std::variant<std::uint8_t*, std::int8_t*, std::uint16_t*, std::int16_t*, std::uint32_t*, std::int32_t*, std::uint64_t*, std::int64_t*> ptr;
        };
        using IndexArray = std::vector<Index>;
        IndexArray _indices;
        using Buffer = std::vector<std::byte>;
        Buffer _buffer;
        std::size_t _nextIndex{0};
        template<typename T>
        void* alignPtr(void*& ptr, std::size_t& available)
        {
            available = sizeof(T) + alignof(T);

            return std::align(alignof(T), sizeof(T), ptr, available);
        }
        template<typename T>
        void insert(const Value& value, void* ptr, std::size_t& available)
        {
            new (ptr) T(std::get<T>(value.value()));
            Index entry{};
            entry.index = static_cast<std::byte*>(ptr) - _buffer.data();
            entry.type = value.type();
            _indices.emplace_back(entry);
        }
    };
}
