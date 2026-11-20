//
// Created by Tony Horrobin on 19/11/2026.
//

#include "config/config.h"

#include "core/Arguments.h"
#include "core/Value.h"

namespace dagbase
{
    Arguments::Arguments(std::size_t n)
    {
        _buffer.resize(n);
    }

    void Arguments::append(const Value &value)
    {
        std::size_t size=0;
        switch (value.type())
        {
            case Value::TYPE_UINT8:
                size = sizeof(std::uint8_t) + alignof(std::uint8_t);
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
        auto oldSize = _buffer.size();
        if (_nextIndex + size > oldSize)
        {
            _buffer.resize(_nextIndex+size);
        }
        void* ptr = _buffer.data() + _nextIndex;
        _nextIndex += size;
        if (_nextIndex<_buffer.size())
        {
            std::size_t available = _nextIndex - size;
            switch (value.type())
            {
                case Value::TYPE_UINT8:
                    insert<std::uint8_t>(value, ptr, available);
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
                    insert<std::int64_t>(value, ptr, available);

                    break;
            }
        }
    }

    void Arguments::get(std::uint32_t index, Value *value)
    {
        if (value && index < _indices.size())
        {
            switch (_indices[index].type)
            {
                case Value::TYPE_UINT8:
                    *value = *std::launder(reinterpret_cast<std::uint8_t*>(_buffer.data() + _indices[index].index));
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
                    *value = *std::launder(reinterpret_cast<std::int64_t*>(_buffer.data() + _indices[index].index));

                    break;
            }

        }
    }
}
