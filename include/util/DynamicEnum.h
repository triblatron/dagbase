//
// Created by Tony Horrobin on 20/11/2025.
//

#ifndef DAGUI_DYNAMICENUM_H
#define DAGUI_DYNAMICENUM_H

#include "config/DagBaseExport.h"

#include "core/Atom.h"
#include "util/VectorMap.h"

namespace dagbase
{
    class ConfigurationElement;

    class DAGBASE_API DynamicEnum
    {
    public:
        DynamicEnum() = default;

        void configure(ConfigurationElement& config);

        std::uint32_t lookup(Atom name) const;

        std::uint32_t lookupOne(Atom name) const;
    private:
        using Lookup = VectorMap<Atom, std::uint32_t>;
        Lookup _lookup;
    };
}

#endif //DAGUI_DYNAMICENUM_H
