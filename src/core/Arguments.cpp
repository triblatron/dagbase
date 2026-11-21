//
// Created by Tony Horrobin on 19/11/2026.
//

#include "config/config.h"

#include "core/Arguments.h"
#include "core/Value.h"

#include <string>

namespace dagbase
{
    Arguments::Arguments(std::size_t n)
    {
        _buffer.resize(n);
    }

    void Arguments::append(const Value &value)
    {
        std::size_t size=0;
        auto oldSize = _buffer.size();
        void* ptr = _buffer.data() + _nextIndex;
        std::size_t available = 0;//_nextIndex + size - oldSize;

        switch (value.type())
        {
            case Value::TYPE_UINT8:
                appendTemplate<std::uint8_t>(value);
                break;
            case Value::TYPE_INT8:
                appendTemplate<std::int8_t>(value);
                break;
            case Value::TYPE_UINT16:
                appendTemplate<std::uint16_t>(value);
                break;
            case Value::TYPE_INT16:
                appendTemplate<std::int16_t>(value);
                break;
            case Value::TYPE_UINT32:
                appendTemplate<std::uint32_t>(value);
                break;
            case Value::TYPE_INT32:
                appendTemplate<std::int32_t>(value);
                break;
            case Value::TYPE_UINT64:
                appendTemplate<std::uint64_t>(value);
                break;
            case Value::TYPE_INT64:
                appendTemplate<std::int64_t>(value);
                break;
            case Value::TYPE_FLOAT:
                appendTemplate<float>(value);
                break;
            case Value::TYPE_DOUBLE:
                appendTemplate<double>(value);
                break;
            case Value::TYPE_BOOL:
                appendTemplate<bool>(value);
                break;
            case Value::TYPE_STRING:
                appendTemplate<std::string*>(value);
                break;
            case Value::TYPE_VEC2:
                break;
            case Value::TYPE_OPAQUE:
                break;
            case Value::TYPE_VECTOR:
                break;
            case Value::TYPE_ENUM:
                break;
            case Value::TYPE_PORT:
                break;
            case Value::TYPE_UNKNOWN:
                break;
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
                    *value = *std::launder(reinterpret_cast<std::int8_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_UINT16:
                    *value = *std::launder(reinterpret_cast<std::uint16_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_INT16:
                    *value = *std::launder(reinterpret_cast<std::int16_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_UINT32:
                    *value = *std::launder(reinterpret_cast<std::uint32_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_INT32:
                    *value = *std::launder(reinterpret_cast<std::int32_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_UINT64:
                    *value = *std::launder(reinterpret_cast<std::uint64_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_INT64:
                    *value = *std::launder(reinterpret_cast<std::int64_t*>(_buffer.data() + _indices[index].index));

                    break;
                case Value::TYPE_FLOAT:
                    *value = *std::launder(reinterpret_cast<float*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_DOUBLE:
                    *value = *std::launder(reinterpret_cast<double*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_BOOL:
                    *value = *std::launder(reinterpret_cast<bool*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_STRING:
                    *value = *std::launder(reinterpret_cast<std::string**>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_VEC2:
                    break;
                case Value::TYPE_OPAQUE:
                    break;
                case Value::TYPE_VECTOR:
                    break;
                case Value::TYPE_ENUM:
                    break;
                case Value::TYPE_PORT:
                    break;
                case Value::TYPE_UNKNOWN:
                    break;
            }

        }
    }
}
