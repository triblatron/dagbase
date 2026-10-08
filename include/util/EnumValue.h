//
// Created by Tony Horrobin on 22/09/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include <functional>
#include <cstdint>
#include <string>

namespace dagbase
{
    struct Type;
    class InputStream;
    class OutputStream;

    class DAGBASE_API EnumValue
    {
    public:
        enum Enum
        {
            ENUM_VARIANT_TYPE,
            ENUM_VALUE_TYPE
        };
        using StringConverter = std::function<const char*(std::uint32_t value)>;
        using Parser = std::function<std::uint32_t(const char*)>;
    public:
        EnumValue() = default;

        explicit EnumValue(Type* type, const char* str=nullptr);

        void set(const char* str);

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

        std::string toString() const;

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

        OutputStream& writeToStream(OutputStream& str) const;

        InputStream& readFromStream(InputStream& str);
    private:
        Type* _type{nullptr};
        std::uint32_t _value{};
    };
}
