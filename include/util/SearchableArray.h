//
// Created by Tony Horrobin on 18/04/2025.
//

#pragma once

#include "core/ConfigurationElement.h"
#include "util/Searchable.h"

namespace dagbase
{
    template<typename Array>
    class SearchableArray
    {
    public:
        using value_type = typename Array::value_type;
        using iterator = typename Array::iterator;
        using const_iterator = typename Array::const_iterator;

        Array a;

        std::size_t size() const
        {
            return a.size();
        }

        iterator begin()
        {
            return a.begin();
        }

        const_iterator begin() const
        {
            return a.begin();
        }

        iterator end()
        {
            return a.end();
        }

        const_iterator end() const
        {
            return a.end();
        }

        void configure(ConfigurationElement& config)
        {
            config.eachChild([this](ConfigurationElement& child) {
                value_type value;
                value.configure(child);
                a.emplace_back(value);
                return true;
            });
        }

        Variant find(std::string_view path) const
        {
            ConfigurationElement::ValueType retval;

            retval = findArray(path, a);
            if (retval.has_value())
                return retval;

            return {};
        }

        bool operator==(const SearchableArray<Array>& other) const
        {
            return a == other.a;
        }
    };

    template<typename Array, typename ConfiguredType=typename Array::value_type>
    class SearchablePrimitiveArray
    {
    public:
        using value_type = typename Array::value_type;
        Array a;

        void configure(ConfigurationElement& config)
        {
            config.eachChild([this](ConfigurationElement& child) {
                const auto& value = child.as<ConfiguredType>();
                a.emplace_back(value_type(value));
                return true;
            });
        }

        Variant find(std::string_view path) const
        {
            Variant retval;

            retval = findPrimitiveArray(path, a);
            if (retval.has_value())
                return retval;

            return {};
        }
    };
}