//
// Created by Tony Horrobin on 19/11/2026.
//

#include "config/config.h"

#include "core/Arguments.h"
#include "core/Value.h"

namespace dagbase
{
    void Arguments::append(const Value &value)
    {
        std::size_t size=0;
        switch (value.type())
        {
            case Value::TYPE_UINT8:
                break;
            case Value::TYPE_INT8:
                break;
            case Value::TYPE_UINT16:
                break;
            case Value::TYPE_INT16:
                break;
            case Value::TYPE_UINT32:
                break;
            case Value::TYPE_INT32:
                break;
            case Value::TYPE_UINT64:
                break;
            case Value::TYPE_INT64:
                size = sizeof(std::int64_t) + alignof(std::int64_t);
                break;
        }
        std::size_t oldSize = _buffer.size();
        _buffer.resize(_buffer.size() + size);
        void* ptr = _buffer.data() + oldSize;
        std::size_t available = _buffer.size() - oldSize;
        switch (value.type())
        {
            case Value::TYPE_UINT8:
                break;
            case Value::TYPE_INT8:
                break;
            case Value::TYPE_UINT16:
                break;
            case Value::TYPE_INT16:
                break;
            case Value::TYPE_UINT32:
                break;
            case Value::TYPE_INT32:
                break;
            case Value::TYPE_UINT64:
                break;
            case Value::TYPE_INT64:
                std::align(alignof(std::int64_t), sizeof(std::int64_t), ptr, available);
                new (ptr) std::int64_t(std::get<int64_t>(value.value()));
                Index entry;
                entry.ptr = static_cast<std::int64_t*>(ptr);
                _indices.emplace_back(entry);

                break;
        }
    }

    void Arguments::get(std::uint32_t index, Value *value)
    {
        if (value && index < _indices.size())
        {
            switch (_indices[index].ptr.index())
            {
                case Value::TYPE_UINT8:
                    break;
                case Value::TYPE_INT8:
                    break;
                case Value::TYPE_UINT16:
                    break;
                case Value::TYPE_INT16:
                    break;
                case Value::TYPE_UINT32:
                    break;
                case Value::TYPE_INT32:
                    break;
                case Value::TYPE_UINT64:
                    break;
                case Value::TYPE_INT64:
                    *value = *std::get<std::int64_t*>(_indices[index].ptr);

                    break;
            }

        }
    }
}
