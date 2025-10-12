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

        void reserve(std::size_t n)
        {
            m.reserve(n);
        }

        template<typename... Args>
        std::pair<typename Map::iterator,bool> emplace(Args&&... args)
        {
            return m.emplace(std::forward<Args>(args)...);
        }

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

    //! A wrapper for a map of type Map, adding a find(std::string_view) to participate in attribute resolution.
    //! \note We don't provide wrappers for find(const key_type&) because of a conflict with the above
    //! This causes overload resolution to fail in findInternal() and requires an ugly cast to avoid compiler errors.
    //! Instead, we expose the underlying map as a public member variable and require client code to access it to get
    //! usual begin(), end(), find() etc.
    //! \note We provide types for the value, key and mapped types of the underlying container, assuming it
    //! exposes them.
    template<typename Map>
    class SearchableMapFromAtom
    {
    public:
        using value_type = typename Map::value_type;
        using key_type = typename Map::key_type;
        using mapped_type = typename Map::mapped_type;
        using iterator = typename Map::iterator;
        using const_iterator = typename Map::const_iterator;
    public:
        SearchableMapFromAtom() = default;

        //! \return The number of elements in the underlying Map
        std::size_t size() const
        {
            return m.size();
        }

        //! \note Assumes that Map is a vector-like class such as VectorMap
        //! \return m.capacity()
        std::size_t capacity() const
        {
            return m.capacity();
        }

        //! Allocate memory for n elements so that capacity() >= n
        //! \note Assumes mapped_type is a vector-like class such as VectorMap
        void reserve(std::size_t n)
        {
            m.reserve(n);
        }

        template<typename... Args>
        std::pair<typename Map::iterator,bool> emplace(Args&&... args)
        {
            return m.emplace(std::forward<Args>(args)...);
        }

        //! \note Assumes mapped_type is a pointer.
        mapped_type lookup(const key_type& key)
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
    class SearchableMapFromAtomPair
    {
    public:
        using value_type = typename Map::value_type;
        using key_type = typename Map::key_type;
        using mapped_type = typename Map::mapped_type;
    public:
        SearchableMapFromAtomPair() = default;

        std::size_t size() const
        {
            return m.size();
        }

        void reserve(std::size_t n)
        {
            m.reserve(n);
        }

        template<typename... Args>
        std::pair<typename Map::iterator,bool> emplace(Args&&... args)
        {
            return m.emplace(std::forward<Args>(args)...);
        }

        //! \note Assumes mapped_type is a pointer
        mapped_type lookup(const key_type& key)
        {
            if (auto it=m.find(key); it!=m.end())
                return it->second;

            return nullptr;
        }

        dagbase::Variant find(std::string_view path) const
        {
            dagbase::ConfigurationElement::ValueType retval;

            retval = findMapFromAtomPair(path, m);
            if (retval.has_value())
                return retval;

            return {};
        }
        Map m;
    };
}
