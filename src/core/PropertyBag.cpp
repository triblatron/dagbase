//
// Created by Tony Horrobin on 20/05/2025.
//

#include "config/config.h"

#include "core/PropertyBag.h"

namespace dagbase
{

    Variant PropertyBag::property(std::string_view path) const
    {
        Variant retval;

        Atom key = Atom::intern(path.data());
        if (auto it=_dynamicProps.find(key); it!=_dynamicProps.end())
        {
            return it->second;
        }

        return {};
    }

    void PropertyBag::configure(ConfigurationElement &config)
    {
        if (auto element = config.findElement("dynamicProperties"); element)
        {
            element->eachChild([this](ConfigurationElement& child) {
                _dynamicProps.emplace(Atom::intern(child.name()), child.value());
                return true;
            });
        }

    }
}
