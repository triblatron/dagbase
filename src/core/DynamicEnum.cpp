//
// Created by Tony Horrobin on 20/11/2025.
//

#include "config/config.h"

#include "util/DynamicEnum.h"
#include "core/ConfigurationElement.h"

namespace dagbase
{
    void DynamicEnum::configure(ConfigurationElement &config)
    {
        config.eachChild([this](ConfigurationElement& child) {
            std::string name;
            dagbase::ConfigurationElement::readConfig(child, "name", &name);
            std::uint32_t value{0};
            dagbase::ConfigurationElement::readConfig(child, "value", &value);
            _lookup.emplace(dagbase::Atom::intern(name), value);

            return true;
        });
    }

    std::uint32_t DynamicEnum::lookup(Atom name) const
    {
        if (auto it=_lookup.find(name); it!=_lookup.end())
        {
            return it->second;
        }

        return 0;
    }
}
