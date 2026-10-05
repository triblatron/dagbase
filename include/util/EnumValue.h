//
// Created by Tony Horrobin on 22/09/2026.
//

#pragma once

#include <optional>

#include "config/DagBaseExport.h"

#include <functional>
#include <cstdint>

namespace dagbase
{
    class DAGBASE_API EnumValue
    {
    public:
        using StringConverter = std::function<const char*(std::uint32_t value)>;
        using Parser = std::function<std::uint32_t(const char*)>;
    public:
        EnumValue(StringConverter stringConverter, Parser parser)
            :
        _stringConverter(std::move(stringConverter)),
        _parser(std::move(parser))
        {
            // Do nothing.
        }

        void set(const char* str)
        {
            if (_parser)
                _value = _parser(str);
            else
                _value = 0;
        }

        template<typename E>
        void set(E value)
        {
            _value = value;
        }

        template<typename E>
        E get() const
        {
            return static_cast<E>(_value);
        }

        const char* toString() const
        {
            if (_stringConverter)
                return _stringConverter(_value);
            return "<error>";
        }

        bool operator<(const EnumValue& other) const
        {
            return _value < other._value;
        }

        bool operator<=(const EnumValue& other) const
        {
            return _value <= other._value;
        }

        bool operator>(const EnumValue& other) const
        {
            return _value > other._value;
        }

        bool operator>=(const EnumValue& other) const
        {
            return _value >= other._value;
        }

        bool operator==(const EnumValue& other) const
        {
            return _value == other._value;
        }

        bool operator!=(const EnumValue& other) const
        {
            return _value != other._value;
        }

    private:
        StringConverter _stringConverter;
        Parser _parser;
        std::uint32_t _value{};
    };
}
