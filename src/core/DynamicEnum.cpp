//
// Created by Tony Horrobin on 20/11/2025.
//

#include "config/config.h"

#include "util/DynamicEnum.h"
#include "core/ConfigurationElement.h"

#include <cctype>

namespace dagbase
{
    void DynamicEnum::configure(ConfigurationElement &config)
    {
        _lookup.reserve(config.numChildren());
        config.eachChild([this](ConfigurationElement& child) {
            std::string name = child.name();
            
            std::uint32_t value = child.asInteger();
            
            _lookup.emplace(dagbase::Atom::intern(name), value);

            return true;
        });
    }

    std::uint32_t DynamicEnum::lookup(Atom name) const
    {
        std::string token;
        std::uint32_t value{0};
        for (auto c : name)
        {
            if (std::isalnum(c) || std::ispunct(c))
            {
                token += c;
            }
            else if (c == ' ')
            {
                value |= lookupOne(dagbase::Atom::intern(token));
                token.clear();
            }
        }
        if (!token.empty())
        {
            value |= lookupOne(dagbase::Atom::intern(token));
        }

        return value;
    }

    std::uint32_t DynamicEnum::lookupOne(Atom name) const
    {
        if (auto it=_lookup.find(name); it!=_lookup.end())
            return it->second;
        else
            return 0;
    }
}
