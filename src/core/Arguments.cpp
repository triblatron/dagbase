//
// Created by Tony Horrobin on 19/11/2026.
//

#include "config/config.h"

#include "core/Arguments.h"
#include "core/Value.h"

#include <memory>
#include <new>
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
                size = sizeof(std::uint8_t);
                alignPtr<std::uint8_t>(ptr, available);
                break;
            case Value::TYPE_INT8:
                size = sizeof(std::int8_t);
                alignPtr<std::int8_t>(ptr, available);
                break;
            case Value::TYPE_UINT16:
                size = sizeof(std::uint16_t);
                alignPtr<std::uint16_t>(ptr, available);
                break;
            case Value::TYPE_INT16:
                size = sizeof(std::int16_t);
                alignPtr<std::int16_t>(ptr, available);
                break;
            case Value::TYPE_UINT32:
                size = sizeof(std::uint32_t);
                alignPtr<std::uint32_t>(ptr, available);
                break;
            case Value::TYPE_INT32:
                size = sizeof(std::int32_t);
                alignPtr<std::int32_t>(ptr, available);
                break;
            case Value::TYPE_UINT64:
                size = sizeof(std::uint64_t);
                alignPtr<std::uint64_t>(ptr, available);
                break;
            case Value::TYPE_INT64:
                size = sizeof(std::int64_t);
                alignPtr<std::int64_t>(ptr, available);
                break;
            case Value::TYPE_FLOAT:
                size = sizeof(float);
                alignPtr<float>(ptr, available);
                break;
            case Value::TYPE_DOUBLE:
                size = sizeof(double);
                alignPtr<double>(ptr, available);
                break;
            case Value::TYPE_BOOL:
                size = sizeof(bool);
                alignPtr<bool>(ptr, available);
                break;
            case Value::TYPE_STRING:
                size = sizeof(std::string*);
                alignPtr<std::string*>(ptr, available);
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
        auto bytePtr = static_cast<std::byte*>(ptr);
        auto newSize = bytePtr + size - _buffer.data();
        if (newSize > static_cast<std::ptrdiff_t>(oldSize))
        {
            std::byte* oldBuf = _buffer.data();
            _buffer.resize(newSize);
            ptr = _buffer.data() + (bytePtr - oldBuf);
        }
        _nextIndex = newSize;
        if (_nextIndex<=_buffer.size())
        {
            switch (value.type())
            {
                case Value::TYPE_UINT8:
                    insert<std::uint8_t>(value, ptr, available);
                    break;
                case Value::TYPE_INT8:
                    insert<std::int8_t>(value, ptr, available);
                    break;
                case Value::TYPE_UINT16:
                    insert<std::uint16_t>(value, ptr, available);
                    break;
                case Value::TYPE_INT16:
                    insert<std::int16_t>(value, ptr, available);
                    break;
                case Value::TYPE_UINT32:
                    insert<std::uint32_t>(value, ptr, available);
                    break;
                case Value::TYPE_INT32:
                    insert<std::int32_t>(value, ptr, available);
                    break;
                case Value::TYPE_UINT64:
                    insert<std::uint64_t>(value, ptr, available);
                    break;
                case Value::TYPE_INT64:
                    insert<std::int64_t>(value, ptr, available);

                    break;
                case Value::TYPE_FLOAT:
                    insert<float>(value, ptr, available);
                    break;
                case Value::TYPE_DOUBLE:
                    insert<double>(value, ptr, available);
                    break;
                case Value::TYPE_BOOL:
                    insert<bool>(value, ptr, available);
                    break;
                case Value::TYPE_STRING:
                    insert<std::string*>(value, ptr, available);
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
