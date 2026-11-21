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

    dagbase::Status Arguments::get(std::uint32_t index, Value *value) const
    {
        dagbase::Status status{dagbase::Status::STATUS_OK};
        if (value && index < _indices.size())
        {
            auto offset = _indices[index].index;
            switch (_indices[index].type)
            {
                case Value::TYPE_UINT8:
                    *value = load<std::uint8_t>(offset);//*std::launder(reinterpret_cast<const std::uint8_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_INT8:
                    *value = load<std::int8_t>(offset);//*std::launder(reinterpret_cast<const std::int8_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_UINT16:
                    *value = load<std::uint16_t>(offset);//*std::launder(reinterpret_cast<const std::uint16_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_INT16:
                    *value = load<std::int16_t>(offset);//*std::launder(reinterpret_cast<const std::int16_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_UINT32:
                    *value = load<std::uint32_t>(offset);//*std::launder(reinterpret_cast<const std::uint32_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_INT32:
                    *value = load<std::int32_t>(offset);//*std::launder(reinterpret_cast<const std::int32_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_UINT64:
                    *value = load<std::uint64_t>(offset);//*std::launder(reinterpret_cast<const std::uint64_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_INT64:
                    *value = load<std::int64_t>(offset);//*std::launder(reinterpret_cast<const std::int64_t*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_FLOAT:
                    *value = load<float>(offset);//*std::launder(reinterpret_cast<const float*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_DOUBLE:
                    *value = load<double>(offset);//*std::launder(reinterpret_cast<const double*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_BOOL:
                    *value = load<bool>(offset);//*std::launder(reinterpret_cast<const bool*>(_buffer.data() + _indices[index].index));
                    break;
                case Value::TYPE_STRING:
                    *value = load<std::string*>(_indices[index].index);
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
        else if (index >= _indices.size())
        {
            status.status = dagbase::Status::STATUS_ARGUMENT_OUT_OF_RANGE;
        }
        else if (!value)
        {
            status.status = dagbase::Status::STATUS_ARGUMENT_NULL;
        }
    }
}
