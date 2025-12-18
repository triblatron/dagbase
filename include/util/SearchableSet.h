//
// Created by Tony Horrobin on 18/12/2025.
//

#pragma once

#include "core/ConfigurationElement.h"

#include <cstdlib>
#include <utility>

namespace dagbase
{
    template<typename Set>
    class SearchableSet
    {
    public:
        using value_type = typename Set::value_type;
        using iterator = typename Set::iterator;
        using const_iterator = typename Set::const_iterator;
    public:
        SearchableSet() = default;

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
        std::pair<typename Set::iterator,bool> emplace(Args&&... args)
        {
            return m.emplace(std::forward<Args>(args)...);
        }

        //! \note Assumes mapped_type is a pointer.
        value_type lookup(const value_type& key)
        {
            if (auto it=m.find(key); it!=m.end())
                return it->second;

            return nullptr;
        }

        dagbase::Variant find(std::string_view path) const
        {
            dagbase::ConfigurationElement::ValueType retval;

            retval = findSetFromAtom(path, m);
            if (retval.has_value())
                return retval;

            return {};
        }
        Set m;
    };

}