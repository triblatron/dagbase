//
// Created by Tony Horrobin on 20/05/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include <functional>

namespace dagbase
{
    class ConfigurationElement;
    class PropertyBag;

    class DAGBASE_API PropertyBagFactory
    {
    public:
        PropertyBag* create(ConfigurationElement& config);
        using UnknownClassHandler = std::function<PropertyBag*(const std::string&, ConfigurationElement&)>;

        void setUnknownClassHandler(UnknownClassHandler handler)
        {
            _unknownClassHandler = handler;
        }
    private:
        UnknownClassHandler _unknownClassHandler;
    };
}