//
// Created by Tony Horrobin on 27/04/2025.
//

#include "config/config.h"

#include "core/Variant.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"
#include "util/Searchable.h"
#include "util/enums.h"
#include "core/Function.h"
#include "core/LuaInterface.h"

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
        //str.writeHeader("Variant");
        str.writeBool(has_value());
        if (has_value())
        {
            str.writeUInt8(std::uint8_t(_value->index()));
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
                case Variant::TYPE_FUNCTION:
                    std::get<TYPE_FUNCTION>(_value.value())->write(str);
                    break;
            }

        }

        //str.writeFooter();

        return str;
    }

    bool Variant::operator!=(const Variant& other) const
    {
        if (has_value())
        {
            if (index() == other.index())
            {
                if (index() != TYPE_FUNCTION)
                {
                    return _value != other._value;
                }
                else
                {
                    return !std::get<TYPE_FUNCTION>(_value.value())->equals(*std::get<TYPE_FUNCTION>(other._value.value()));
                }
            }
            else
                return true;
        }
        else
        {
            return other.has_value();
        }
    }

    InputStream &Variant::read(InputStream &str, Lua* lua)
    {
//        std::string name;
//        str.readHeader(&name);
        bool hasValue{false};
        str.readBool(&hasValue);
        if (hasValue)
        {
            std::uint8_t type{0};
            str.readUInt8(&type);
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
                case Variant::TYPE_FUNCTION:
                {
                    _value = new Function(str, *lua);
                    break;
                }
                default:
                    break;
            }

        }

        //str.readFooter();
        return str;
    }

    Variant Variant::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "value", *this);
        if (retval.has_value())
            return retval;

        return {};
    }

    const char *Variant::indexToString(Variant::Index value)
    {
        switch (value)
        {
            ENUM_NAME(TYPE_INTEGER)
            ENUM_NAME(TYPE_DOUBLE)
            ENUM_NAME(TYPE_BOOL)
            ENUM_NAME(TYPE_STRING)
            ENUM_NAME(TYPE_COLOUR)
            ENUM_NAME(TYPE_VEC2)
            ENUM_NAME(TYPE_UINT)
            ENUM_NAME(TYPE_FUNCTION)
            ENUM_NAME(TYPE_UNKNOWN)
        }

        return "<error>";
    }

    Variant::Index Variant::parseIndex(const char *str)
    {
        TEST_ENUM(TYPE_INTEGER, str)
        TEST_ENUM(TYPE_DOUBLE, str)
        TEST_ENUM(TYPE_BOOL, str)
        TEST_ENUM(TYPE_STRING, str)
        TEST_ENUM(TYPE_COLOUR, str)
        TEST_ENUM(TYPE_VEC2, str)
        TEST_ENUM(TYPE_UINT, str)
        TEST_ENUM(TYPE_FUNCTION, str)

        return Variant::TYPE_UNKNOWN;
    }

    Variant::Variant(Function *value)
    :
    _value(value)
    {
        // Do nothing.
    }
}

std::ostream &operator<<(std::ostream &str, const dagbase::Variant& value)
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
        default:
            str << "<unknown>";
    }
    return str;
}
