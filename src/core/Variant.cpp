//
// Created by Tony Horrobin on 27/04/2025.
//

#include "config/config.h"

#include "core/Variant.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"

#include <iostream>

namespace dagbase
{

    std::string Variant::toString() const
    {
        if (_value.has_value())
        {
            switch (_value->index())
            {
                case TYPE_INTEGER:
                    return std::to_string(std::get<TYPE_INTEGER>(_value.value()));
                case TYPE_DOUBLE:
                    return std::to_string(std::get<TYPE_DOUBLE>(_value.value()));
                case TYPE_BOOL:
                    return std::to_string(std::get<TYPE_BOOL>(_value.value()));
                case TYPE_STRING:
                    return std::get<TYPE_STRING>(_value.value());
                case TYPE_COLOUR:
                    return to_string(std::get<TYPE_COLOUR>(_value.value()));
                case TYPE_VEC2:
                    return to_string(std::get<TYPE_VEC2>(_value.value()));
                case TYPE_UINT:
                    return std::to_string(std::get<TYPE_UINT>(_value.value()));
            }
        }

        return {};
    }

    Variant::Variant(const std::string &strValue)
    :
    _value(strValue)
    {
        // Do nothing.
    }

    Variant::Variant(std::int64_t intValue)
    :
    _value(intValue)
    {
        // Do nothing.
    }

    Variant::Variant(double value)
    :
    _value(value)
    {
        // Do nothing.
    }

    Variant::Variant(bool value)
    :
    _value(value)
    {
        // Do nothing.
    }

    Variant::Variant(const Colour &value)
    :
    _value(value)
    {
        // Do nothing.
    }

    Variant::Variant(const Vec2 &value)
    :
    _value(value)
    {
        // Do nothing.
    }

    Variant::Variant(std::uint32_t value)
    :
    _value(value)
    {
        // Do nothing.
    }

    OutputStream &Variant::write(OutputStream &str) const
    {
        str.writeBool(has_value());
        if (has_value())
        {
            str.writeUInt32(_value->index());
            switch (_value->index())
            {
                case Variant::TYPE_DOUBLE:
                    str.writeDouble(asDouble());
                    break;
                case Variant::TYPE_STRING:
                    str.writeString(asString(),true);
                    break;
                case Variant::TYPE_BOOL:
                    str.writeBool(asBool());
                    break;
                case Variant::TYPE_INTEGER:
                    str.writeInt64(asInteger());
                    break;
                case Variant::TYPE_UINT:
                    str.writeUInt32(asUint32());
                    break;
            }

        }

        return str;
    }

    InputStream &Variant::read(InputStream &str)
    {
        bool hasValue{false};
        str.readBool(&hasValue);
        if (hasValue)
        {
            std::uint32_t type{0};
            str.readUInt32(&type);
            switch (type)
            {
                case Variant::TYPE_DOUBLE:
                {
                    double doubleValue{0.0};
                    str.readDouble(&doubleValue);
                    _value = doubleValue;
                    break;
                }
                case Variant::TYPE_STRING:
                {
                    std::string stringValue;
                    str.readString(&stringValue, true);
                    _value = stringValue;
                    break;
                }
                case Variant::TYPE_BOOL:
                {
                    bool boolValue{false};
                    str.readBool(&boolValue);
                    _value = boolValue;
                    break;
                }
                case Variant::TYPE_INTEGER:
                {
                    int64_t int64Value{0};
                    str.readInt64(&int64Value);
                    _value = int64Value;
                    break;
                }
                case Variant::TYPE_UINT:
                {
                    std::uint32_t uint32Value{0};
                    str.readUInt32(&uint32Value);
                    _value = uint32Value;
                    break;
                }
                default:
                    break;
            }

        }

        return str;
    }
}

std::ostream &operator<<(std::ostream &str, dagbase::Variant value)
{
    switch (value.index())
    {
        case dagbase::Variant::TYPE_INTEGER:
            str << value.asInteger();
            break;
        case dagbase::Variant::TYPE_BOOL:
            str << bool(value.asBool());
            break;
        case dagbase::Variant::TYPE_DOUBLE:
            str << value.asDouble();
            break;
        case dagbase::Variant::TYPE_STRING:
            str << value.asString();
            break;
        case dagbase::Variant::TYPE_COLOUR:
            str << dagbase::to_string(value.asColour());
            break;
        case dagbase::Variant::TYPE_VEC2:
            str << dagbase::to_string(value.asVec2());
            break;
        case dagbase::Variant::TYPE_UINT:
            str << value.asUint32();
            break;
    }
    return str;
}
