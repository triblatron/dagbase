//
// Created by Tony Horrobin on 25/09/2025.
//

#include "config/config.h"

#include "util/DynamicMultiArray.h"
#include "core/ConfigurationElement.h"

namespace dagbase
{
    void DynamicMultiArray::configure(dagbase::ConfigurationElement &config)
    {
        if (auto element=config.findElement("arrays"); element)
        {
            _arrays.a.reserve(element->numChildren());
            element->eachChild([this](dagbase::ConfigurationElement& child) {
                Arrays::value_type a;
                a.reserve(child.numChildren());
                a.configure(child);
                _arrays.a.emplace_back(a);

                return true;
            });
        }
    }

    void DynamicMultiArray::emplace_back(const VariantArray &input)
    {
        std::size_t i=0;
        std::for_each(_arrays.begin(), _arrays.end(),[&input, &i](Arrays::value_type& a) {
            a.emplace_back(input[i]);
            i++;
        });
    }

    Variant DynamicMultiArray::find(std::string_view path) const
    {
        Variant retval;

        retval = findInternal(path, "arrays", _arrays);
        if (retval.has_value())
            return retval;

        return {};
    }
}
