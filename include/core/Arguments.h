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
#include <memory>
#include <new>

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
            std::size_t index{0};
            dagbase::Value::Type type{dagbase::Value::TYPE_UNKNOWN};
            //std::variant<std::uint8_t*, std::int8_t*, std::uint16_t*, std::int16_t*, std::uint32_t*, std::int32_t*, std::uint64_t*, std::int64_t*> ptr;
        };
        using IndexArray = std::vector<Index>;
        IndexArray _indices;
        using Buffer = std::vector<std::byte>;
        Buffer _buffer;
        std::size_t _nextIndex{0};

        template<typename T>
        void appendTemplate(const Value& value)
        {
            // Round offset to next multiple of alignment
            // This works because alignof() always gives a power of two.
            // The -1 in the first part ensures we round up rather than down.
            // The -1 in the mask gives the low order bits and the ~ flips them
            const std::size_t offset = (_nextIndex + alignof(T) - 1) & ~(alignof(T) - 1);
            const std::size_t end = offset + sizeof(T);
            if (end > _buffer.size())
                _buffer.resize(end);

            new (_buffer.data() + offset) T(std::get<T>(value.value()));

            _indices.emplace_back(Index{offset, value.type()});
            _nextIndex = end;
        }

    };
}
