//
// Created by Tony Horrobin on 27/04/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Colour.h"
#include "core/Vec2.h"

#include <optional>
#include <variant>
#include <cstdint>
#include <string>
#include <iosfwd>

namespace dagbase
{
    class Function;
    class OutputStream;
    class InputStream;

    class DAGBASE_API Variant
    {
    public:
        using ValueType = std::optional<std::variant<std::int64_t, double, bool, std::string, Colour, Vec2, std::uint32_t, Function*>>;
        using InnerType = ValueType::value_type;

        enum Index : std::uint32_t
        {
            TYPE_INTEGER,
            TYPE_DOUBLE,
            TYPE_BOOL,
            TYPE_STRING,
            TYPE_COLOUR,
            TYPE_VEC2,
            TYPE_UINT,
            TYPE_FUNCTION,
            TYPE_UNKNOWN
        };
    public:
        Variant() = default;

        explicit Variant(std::int64_t intValue);

        explicit Variant(double value);

        explicit Variant(const std::string& strValue);

        explicit Variant(bool value);

        explicit Variant(const Colour& value);

        explicit Variant(const Vec2& value);

        explicit Variant(std::uint32_t value);

        explicit Variant(Function* value);

        //! Reject conversion from const char* to bool using SFINAE
        template <typename T,
                typename = std::enable_if_t<std::is_convertible_v<T, std::string> &&
                                            !std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, bool>>>
        Variant(T&& val) : _value(std::string(std::forward<T>(val))) {}

        bool operator==(const Variant& other) const
        {
            return _value == other._value;
        }

        bool operator<(const Variant& other) const
        {
            return _value < other._value;
        }

        int64_t asInteger(std::int64_t defaultValue=0) const
        {
            if (_value.has_value() && _value->index() == TYPE_INTEGER)
                return std::get<TYPE_INTEGER>(_value.value());
            else
                return defaultValue;
        }

        double asDouble(double defaultValue=0.0) const
        {
            if (_value.has_value() && _value->index() == TYPE_DOUBLE)
                return std::get<TYPE_DOUBLE>(_value.value());
            else
                return defaultValue;
        }

        bool asBool(bool defaultValue=false) const
        {
            if (_value.has_value() && _value->index() == TYPE_BOOL)
                return std::get<TYPE_BOOL>(_value.value());
            else
                return defaultValue;
        }

        std::string asString(std::string defaultValue="") const
        {
            if (_value.has_value() && _value->index() == TYPE_STRING)
                return std::get<TYPE_STRING>(_value.value());
            else
                return defaultValue;
        }

        Colour asColour(dagbase::Colour defaultValue=Colour()) const
        {
            if (_value.has_value() && _value->index() == TYPE_COLOUR)
                return std::get<TYPE_COLOUR>(_value.value());
            else
                return defaultValue;
        }

        Vec2 asVec2(Vec2 defaultValue=Vec2()) const
        {
            if (_value.has_value() && _value->index() == TYPE_VEC2)
                return std::get<TYPE_VEC2>(_value.value());
            else
                return defaultValue;
        }

        std::uint32_t asUint32(std::uint32_t defaultValue=0) const
        {
            if (_value.has_value() && _value->index() == TYPE_UINT)
                return std::get<TYPE_UINT>(_value.value());
            else
                return defaultValue;
        }

        template<typename T>
        T as() const
        {
            if (_value.has_value())
            {
                return std::get<T>(_value.value());
            }
            else
            {
                return T();
            }
        }

        bool has_value() const
        {
            return _value.has_value();
        }

        Index index() const
        {
            return Index(_value->index());
        }

        ValueType& value()
        {
            return _value;
        }

        const ValueType& value() const
        {
            return _value;
        }

        ValueType::value_type& operator->()
        {
            return _value.value();
        }

        bool operator!=(const Variant& other) const
        {
            return _value != other._value;
        }

        OutputStream& write(OutputStream& str) const;

        InputStream& read(InputStream& str);

        Variant find(std::string_view path) const;

        std::string toString() const;

        static const char* indexToString(Index value);

        static Index parseIndex(const char* str);
    private:
        ValueType _value;
    };
}

std::ostream DAGBASE_API & operator<<(std::ostream& str, const dagbase::Variant& value);
//std::string to_string(dagbase::Variant value);