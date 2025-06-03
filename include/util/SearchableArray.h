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
        Array a;

        ConfigurationElement::ValueType find(std::string_view path) const
        {
            ConfigurationElement::ValueType retval;

            retval = findArray(path, a);
            if (retval.has_value())
                return retval;

            return {};
        }
    };

    template<typename Array>
    class SearchablePrimitiveArray
    {
    public:
        using value_type = typename Array::value_type;
        Array a;

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