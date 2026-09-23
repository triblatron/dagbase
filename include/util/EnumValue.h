//
// Created by Tony Horrobin on 22/09/2026.
//

#pragma once

#include <optional>

#include "config/DagBaseExport.h"

#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <cstdint>

namespace dagbase
{
    class DAGBASE_API EnumValue
    {
    public:
        template<typename E>
        using StringConverter = const char*(*)(E value);
        template<typename E>
        using Parser = E (*)(const char*);
    public:
        template<typename E, std::enable_if_t<std::is_enum_v<E>, bool> = true>
        EnumValue(E e)
            :
        _rawValue(static_cast<std::uint64_t>(static_cast<std::underlying_type_t<E>>(e))),
        _type(typeid(E))
        {
            // Do nothing.
        }

        template<typename E>
        bool holds() const
        {
            static_assert(std::is_enum_v<E>);
            return _type == std::type_index(typeid(E));
        }

        template <typename E>
        std::optional<E> tryGet() const
        {
            static_assert(std::is_enum_v<E>);
            if (!holds<E>())
                return std::nullopt;
            return static_cast<E>(static_cast<std::underlying_type_t<E>>(_rawValue));
        }

        std::uint64_t raw() const
        {
            return _rawValue;
        }

        std::type_index type() const
        {
            return _type;
        }

        bool operator<(const EnumValue& other) const
        {
            return _type == other._type && _rawValue < other._rawValue;
        }

        bool operator<=(const EnumValue& other) const
        {
            return _type == other._type && _rawValue <= other._rawValue;
        }

        bool operator>(const EnumValue& other) const
        {
            return _type == other._type && _rawValue > other._rawValue;
        }

        bool operator>=(const EnumValue& other) const
        {
            return _type == other._type && _rawValue >= other._rawValue;
        }

        bool operator==(const EnumValue& other) const
        {
            return _type == other._type && _rawValue == other._rawValue;
        }

        bool operator!=(const EnumValue& other) const
        {
            return _type != other._type || _rawValue != other._rawValue;
        }
    private:
        std::uint64_t _rawValue;
        std::type_index _type;
    };
}
