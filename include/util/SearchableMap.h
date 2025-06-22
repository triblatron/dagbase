//
// Created by Tony Horrobin on 09/04/2025.
//

#pragma once
#include "core/Variant.h"
#include "util/Searchable.h"

namespace dagbase
{
    template<typename Map>
    class SearchableMap
    {
    public:
        using value_type = typename Map::value_type;
    public:
        SearchableMap() = default;

        // typename Map::iterator insert(const typename Map::InnerType& value)
        // {
        //     return m.insert(value);
        // }

        // typename Map::iterator find(Map::key_type key)
        // {
        //     return m.find(key);
        // }

        dagbase::ConfigurationElement::ValueType find(std::string_view path) const
        {
            dagbase::ConfigurationElement::ValueType retval;

            retval = findMap(path, m);
            if (retval.has_value())
                return retval;

            return {};
        }
        Map m;
    };
    template<typename Map>
    class SearchableMapFromAtom
    {
    public:
        using value_type = typename Map::value_type;
        using mapped_type = typename Map::mapped_type;
    public:
        SearchableMapFromAtom() = default;

        // typename Map::iterator insert(const typename Map::InnerType& value)
        // {
        //     return m.insert(value);
        // }

        // typename Map::iterator find(Map::key_type key)
        // {
        //     return m.find(key);
        // }

        mapped_type lookup(typename Map::key_type key)
        {
            if (auto it=m.find(key); it!=m.end())
                return it->second;

            return nullptr;
        }

        dagbase::Variant find(std::string_view path) const
        {
            dagbase::ConfigurationElement::ValueType retval;

            retval = findMapFromAtom(path, m);
            if (retval.has_value())
                return retval;

            return {};
        }
        Map m;
    };

}
