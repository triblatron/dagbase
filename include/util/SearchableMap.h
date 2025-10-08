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
        using iterator = typename Map::iterator;
        using const_iterator = typename Map::const_iterator;
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
        using key_type = typename Map::key_type;
        using mapped_type = typename Map::mapped_type;
    public:
        SearchableMapFromAtom() = default;

        std::size_t size() const
        {
            return m.size();
        }

        template<typename... Args>
        std::pair<typename Map::iterator,bool> emplace(Args&&... args)
        {
            return m.emplace(std::forward<Args>(args)...);
        }

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

    template<typename Map>
    class SearchableVectorMapFromAtom
    {
    public:
        using value_type = typename Map::value_type;
        using key_type = typename Map::key_type;
        using mapped_type = typename Map::mapped_type;
    public:
        SearchableVectorMapFromAtom() = default;

        void reserve(std::size_t n)
        {
            m.reserve(n);
        }

        mapped_type lookup(typename Map::key_type key)
        {
            if (auto it=m.find(key); it!=m.end())
                return it->second;

            return nullptr;
        }

        std::size_t size() const
        {
            return m.size();
        }

        template<typename... Args>
        std::pair<typename Map::iterator,bool> emplace(Args&&... args)
        {
            return m.emplace(std::forward<Args>(args)...);
        }

        typename Map::iterator find(const key_type & key)
        {
            return m.find(key);
        }

        typename Map::const_iterator find(const key_type & key) const
        {
            return m.find(key);
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
