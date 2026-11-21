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

    Status Arguments::append(const Value &value)
    {
        Status status{Status::STATUS_OK};

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
                appendTemplatePtr<std::string*, std::shared_ptr<std::string>>(value);
                break;
            case Value::TYPE_VEC2:
                appendTemplate<Vec2>(value);
                break;
            case Value::TYPE_OPAQUE:
                // Not supported.
                appendTemplate<void*>(value);
                break;
            case Value::TYPE_VECTOR:
                appendTemplate<std::vector<Value>*>(value);
                break;
            case Value::TYPE_ENUM:
                appendTemplate<EnumValue>(value);
                break;
            case Value::TYPE_PORT:
                appendTemplate<Port*>(value);
                break;
            case Value::TYPE_UNKNOWN:
                break;
        }

        return status;
    }

    Status Arguments::get(std::uint32_t index, Value *value) const
    {
        Status status{Status::STATUS_OK};
        if (value && index < _indices.size())
        {
            auto offset = _indices[index].index;
            switch (_indices[index].type)
            {
                case Value::TYPE_UINT8:
                    *value = load<std::uint8_t>(offset);
                    break;
                case Value::TYPE_INT8:
                    *value = load<std::int8_t>(offset);
                    break;
                case Value::TYPE_UINT16:
                    *value = load<std::uint16_t>(offset);
                    break;
                case Value::TYPE_INT16:
                    *value = load<std::int16_t>(offset);
                    break;
                case Value::TYPE_UINT32:
                    *value = load<std::uint32_t>(offset);
                    break;
                case Value::TYPE_INT32:
                    *value = load<std::int32_t>(offset);
                    break;
                case Value::TYPE_UINT64:
                    *value = load<std::uint64_t>(offset);
                    break;
                case Value::TYPE_INT64:
                    *value = load<std::int64_t>(offset);
                    break;
                case Value::TYPE_FLOAT:
                    *value = load<float>(offset);
                    break;
                case Value::TYPE_DOUBLE:
                    *value = load<double>(offset);
                    break;
                case Value::TYPE_BOOL:
                    *value = load<bool>(offset);
                    break;
                case Value::TYPE_STRING:
                    *value = std::make_shared<std::string>(*load<std::string*>(_indices[index].index));
                    break;
                case Value::TYPE_VEC2:
                    *value = load<Vec2>(_indices[index].index);
                    break;
                case Value::TYPE_OPAQUE:
                    *value = load<void*>(_indices[index].index);
                    break;
                case Value::TYPE_VECTOR:
                    *value = load<std::vector<Value>*>(_indices[index].index);
                    break;
                case Value::TYPE_ENUM:
                    *value = load<EnumValue>(_indices[index].index);
                    break;
                case Value::TYPE_PORT:
                    *value = load<Port*>(_indices[index].index);
                    break;
                case Value::TYPE_UNKNOWN:
                    break;
            }

        }
        else if (index >= _indices.size())
        {
            status.status = Status::STATUS_ARGUMENT_OUT_OF_RANGE;
        }
        else if (!value)
        {
            status.status = Status::STATUS_ARGUMENT_NULL;
        }

        return status;
    }
}
