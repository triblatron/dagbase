//
// Created by Tony Horrobin on 27/04/2025.
//

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

        enum Index : std::uint32_t
        {
            INDEX_INT,
            INDEX_DOUBLE,
            INDEX_BOOL,
            INDEX_STRING,
            INDEX_COLOUR,
            INDEX_VEC2
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

        int64_t asInteger(std::int64_t defaultValue) const
        {
            if (_value.has_value() && _value->index() == INDEX_INT)
                return std::get<INDEX_INT>(_value.value());
            else
                return defaultValue;
        }

        double asDouble(double defaultValue) const
        {
            if (_value.has_value() && _value->index() == INDEX_DOUBLE)
                return std::get<INDEX_DOUBLE>(_value.value());
            else
                return defaultValue;
        }

        bool asBool(bool defaultValue) const
        {
            if (_value.has_value() && _value->index() == INDEX_BOOL)
                return std::get<INDEX_BOOL>(_value.value());
            else
                return defaultValue;
        }

        std::string asString(std::string defaultValue) const
        {
            if (_value.has_value() && _value->index() == INDEX_STRING)
                return std::get<INDEX_STRING>(_value.value());
            else
                return defaultValue;
        }

        Colour asColour(dagbase::Colour defaultValue) const
        {
            if (_value.has_value() && _value->index() == INDEX_COLOUR)
                return std::get<INDEX_COLOUR>(_value.value());
            else
                return defaultValue;
        }

        Vec2 asVec2(Vec2 defaultValue) const
        {
            if (_value.has_value() && _value->index() == INDEX_VEC2)
                return std::get<INDEX_VEC2>(_value.value());
            else
                return defaultValue;
        }

        std::string toString() const;
    private:
        ValueType _value;
    };
}
