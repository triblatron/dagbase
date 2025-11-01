//
// Created by Tony Horrobin on 30/10/2025.
//

#pragma once


#include "config/DagBaseExport.h"

#include "core/Atom.h"
#include "core/Variant.h"
#include "util/VectorMap.h"

namespace dagbase
{
    class ConfigurationElement;

    class DAGBASE_API ParameterLookup
    {
    public:
        using Lookup = VectorMap<Atom,Variant>;
        enum State : std::uint32_t
        {
            STATE_INITIAL,
            STATE_FOUND_OPEN,
        };
        using container = VectorMap<Atom, Variant>;
        using iterator = typename container::iterator;
        using const_iterator = typename container::const_iterator;
    public:
        iterator find(const Atom& key)
        {
            return _lookup.find(key);
        }

        std::pair<iterator, bool> emplace(const Atom& key, Variant value)
        {
            return _lookup.emplace(key, value);
        }

        iterator begin()
        {
            return _lookup.begin();
        }

        const_iterator begin() const
        {
            return _lookup.begin();
        }

        iterator end()
        {
            return _lookup.end();
        }

        const_iterator end() const
        {
            return _lookup.end();
        }

        void configure(ConfigurationElement& config);

        Atom interpolate(const Atom& atom);
    private:
        container _lookup;
    };

}
