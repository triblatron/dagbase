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

namespace dagbase
{
    class DAGBASE_API Variant
    {
    public:
        using ValueType = std::optional<std::variant<std::int64_t, double, bool, std::string, Colour, Vec2>>;
        using value_type = ValueType::value_type;

        enum Index : std::uint32_t
        {
            TYPE_INTEGER,
            TYPE_DOUBLE,
            TYPE_BOOL,
            TYPE_STRING,
            TYPE_COLOUR,
            TYPE_VEC2
        };
    public:
        Variant() = default;

        Variant(std::int64_t intValue);

        Variant(double value);

        Variant(const std::string& strValue);

        explicit Variant(bool value);

        explicit Variant(const Colour& value);

        explicit Variant(const Vec2& value);

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

        template<typename T>
        T as()
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

        std::string toString() const;
    private:
        ValueType _value;
    };
}

std::ostream& operator<<(std::ostream& str, dagbase::Variant value);
