//
// Created by Tony Horrobin on 27/09/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "util/SearchableArray.h"
#include "core/Variant.h"

#include <variant>
#include <cstdint>
#include <string>
#include <string_view>

namespace dagbase
{
    class DAGBASE_API VariantArray
    {
    public:
        //using ValueType = std::optional<std::variant<std::int64_t, double, bool, std::string, Colour, Vec2, std::uint32_t>>;
        using ArrayOfInt64 = SearchableVariantArray<std::vector<std::int64_t>>;
        using ArrayOfDouble = SearchableVariantArray<std::vector<double>>;
        using ArrayOfBool = SearchableVariantArray<std::vector<bool>>;
        using ArrayOfString = SearchableVariantArray<std::vector<std::string>>;
        using ArrayOfColour = SearchableVariantArray<std::vector<Colour>>;
        using ArrayOfVec2 = SearchableVariantArray<std::vector<Vec2>>;
        using ArrayOfUInt32 = SearchableVariantArray<std::vector<std::uint32_t>, std::int64_t>;
        using ValueType = std::variant<ArrayOfInt64, ArrayOfDouble, ArrayOfBool, ArrayOfString, ArrayOfColour, ArrayOfVec2, ArrayOfUInt32>;
    public:
        void reserve(std::size_t n);

        ValueType& value()
        {
            return _value;
        }

        const ValueType& value() const
        {
            return _value;
        }

        //! Put value on the end of our array.
        //! \warning If the currently held type of value does not match that of
        //! the internal array, the internal array will be reinitialised to the same type as value.
        //! This will lead to data loss if the internal array is not empty.
        void emplace_back(const Variant& value)
        {
            if (value.has_value())
            {
                switch (value.index())
                {
                    case Variant::TYPE_INTEGER:
                        if (value.index() != _value.index())
                        {
                            _value = ArrayOfInt64();
                        }
                        std::get<ArrayOfInt64>(_value).a.emplace_back(value.asInteger());
                        break;
                    case Variant::TYPE_DOUBLE:
                        if (value.index() != _value.index())
                        {
                            _value = ArrayOfDouble();
                        }
                        std::get<ArrayOfDouble>(_value).a.emplace_back(value.asDouble());
                        break;
                    case Variant::TYPE_STRING:
                        if (value.index() != _value.index())
                        {
                            _value = ArrayOfString();
                        }
                        std::get<ArrayOfString>(_value).a.emplace_back(value.asString());
                        break;
                    case Variant::TYPE_BOOL:
                        if (value.index() != _value.index())
                        {
                            _value = ArrayOfBool();
                        }
                        std::get<ArrayOfBool>(_value).a.emplace_back(value.asBool());
                        break;
                    case Variant::TYPE_UINT:
                        if (value.index() != _value.index())
                        {
                            _value = ArrayOfUInt32();
                        }
                        std::get<ArrayOfUInt32>(_value).a.emplace_back(value.asUint32());
                    default:
                        break;
                }
            }
        }

        Variant operator[](std::size_t index) const
        {
            switch (_value.index())
            {
                case Variant::TYPE_INTEGER:
                {
                    auto& a = std::get<ArrayOfInt64>(_value).a;
                    if (index < a.size())
                        return Variant(a[index]);
                    break;
                }
                case Variant::TYPE_DOUBLE:
                {
                    auto& a = std::get<ArrayOfDouble>(_value).a;
                    if (index < a.size())
                        return Variant(a[index]);
                    break;
                }
                case Variant::TYPE_STRING:
                {
                    auto& a = std::get<ArrayOfString>(_value).a;
                    if (index < a.size())
                        return a[index];
                    break;
                }
                case Variant::TYPE_BOOL:
                {
                    auto& a = std::get<ArrayOfBool>(_value).a;
                    if (index < a.size())
                        return Variant(a[index]);
                    break;
                }
                case Variant::TYPE_UINT:
                {
                    auto& a = std::get<ArrayOfUInt32>(_value).a;
                    if (index < a.size())
                        return Variant(a[index]);
                    break;
                }
            }

            return {};
        }

        void configure(ConfigurationElement& config);

        Variant find(std::string_view path) const;
    private:
        ValueType _value;
    };
}