//
// Created by Tony Horrobin on 27/04/2025.
//

#include "config/config.h"

#include "core/Variant.h"

namespace dagbase
{

    std::string Variant::toString() const
    {
        if (_value.has_value())
        {
            switch (_value->index())
            {
                case INDEX_INT:
                    return std::to_string(std::get<INDEX_INT>(_value.value()));
                case INDEX_DOUBLE:
                    return std::to_string(std::get<INDEX_DOUBLE>(_value.value()));
                case INDEX_BOOL:
                    return std::to_string(std::get<INDEX_BOOL>(_value.value()));
                case INDEX_STRING:
                    return std::get<INDEX_STRING>(_value.value());
                case INDEX_COLOUR:
                    return to_string(std::get<INDEX_COLOUR>(_value.value()));
                case INDEX_VEC2:
                    return to_string(std::get<INDEX_VEC2>(_value.value()));
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