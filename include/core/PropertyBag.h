//
// Created by Tony Horrobin on 20/05/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Variant.h"
#include "util/SearchableMap.h"
#include "core/Atom.h"

#include <string_view>
#include <unordered_map>

namespace dagbase
{
    class DAGBASE_API PropertyBag
    {
    public:
        virtual ~PropertyBag() = default;

        virtual Variant property(std::string_view path) const;

        virtual void configure(ConfigurationElement& config);
    private:
        using PropertyMap = std::unordered_map<Atom, Variant>;
        PropertyMap _dynamicProps;
    };

    template<typename T>
    class TemplatePropertyBag : public PropertyBag
    {
    public:
        Variant property(std::string_view path) const override
        {
            Variant retval;

            retval = _staticProps.property(path);
            if (retval.has_value())
                return retval;

            retval = PropertyBag::property(path);
            if (retval.has_value())
                return retval;

            return {};
        }

        void configure(ConfigurationElement& config)
        {
            _staticProps.configure(config);
        }

        T& properties()
        {
            return _staticProps;
        }
    private:
        T _staticProps;
    };
}
