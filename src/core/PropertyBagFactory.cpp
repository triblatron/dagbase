//
// Created by Tony Horrobin on 20/05/2025.
//

#include "config/config.h"

#include "core/PropertyBagFactory.h"
#include "core/ConfigurationElement.h"
#include "core/PropertyBag.h"

#include <string>

namespace dagbase
{

    PropertyBag *PropertyBagFactory::create(ConfigurationElement &config)
    {
        std::string className;

        if (auto element = config.findElement("class"); element)
        {
            className = element->asString();
        }

        PropertyBag* retval = nullptr;

        if (_unknownClassHandler)
        {
            retval = _unknownClassHandler(className, config);
        }

        if (retval)
        {
            retval->configure(config);
        }
        return retval;
    }
}