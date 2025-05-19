//
// Created by Tony Horrobin on 27/04/2025.
//

#include "config/config.h"

#include "core/Variant.h"
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
    }
    return str;
}
