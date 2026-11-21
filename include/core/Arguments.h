//
// Created by Tony Horrobin on 19/11/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Value.h"
#include "core/Status.h"

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
        //! \constructor Allocate n bytes in the backing buffer
        //! This can be used to avoid extra allocations if the required space is known
        explicit Arguments(std::size_t n=0);

        //! Append a Value to the internal buffer and add an index entry for it
        //! \note Port and opaque pointer have not been tested yet
        //! \retval Status{STATUS_OK} on success
        //! \retval An error code otherwise
        //! \note Might reallocate by calling resize() on the buffer
        dagbase::Status append(const Value& value);

        //! Get a value at the given index
        //! \note Does not touch value if the index is out of range and returns STATUS_ARGUMENT_OUT_OF_RANGE
        //! \note Does nothing if value is nullptr
        dagbase::Status get(std::uint32_t index, Value *value) const;

        //! \return The number of indices in the index array
        std::size_t size() const
        {
            return _indices.size();
        }

        //! \return The number of bytes in the backing buffer
        std::size_t bufferSize() const
        {
            return _buffer.size();
        }
    private:
        //! Struct to store the index entry
        //! index is the byte offset into the backing buffer
        //! type is the type encoded at that position in the backing buffer
        struct Index
        {
            std::size_t index{0};
            Value::Type type{dagbase::Value::TYPE_UNKNOWN};
        };
        using IndexArray = std::vector<Index>;
        IndexArray _indices;
        using Buffer = std::vector<std::byte>;
        Buffer _buffer;
        //! The index of the next item in the backing buffer,
        //! \note Not necessarily aligned, because we do not know the type yet.
        std::size_t _nextIndex{0};

        //! Helper to append an aligned value of type T in value.
        //! \note Will throw if T is not an alternative in Value.
        //! \note May call resize() if the aligned T does not fit in the backing buffer.
        //! \post indices will contain a new element pointing to the new T in the backing buffer.
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
        template<typename T, typename Ptr>
        void appendTemplatePtr(const Value& value)
        {
            // Round offset to next multiple of alignment
            // This works because alignof() always gives a power of two.
            // The -1 in the first part ensures we round up rather than down.
            // The -1 in the mask gives the low order bits and the ~ flips them
            const std::size_t offset = (_nextIndex + alignof(T) - 1) & ~(alignof(T) - 1);
            const std::size_t end = offset + sizeof(T);
            if (end > _buffer.size())
                _buffer.resize(end);

            new (_buffer.data() + offset) T(std::get<Ptr>(value.value()).get());

            _indices.emplace_back(Index{offset, value.type()});
            _nextIndex = end;
        }

        //! Helper to load a value from the buffer at offset bytes
        //! \note Fixes the mismatched overload error for std::string* arguments.
        template<typename T>
        const T& load(std::size_t offset) const
        {
            return *std::launder(reinterpret_cast<const T*>(_buffer.data() + offset));
        }
    };
}
