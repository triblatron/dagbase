//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "core/TypeRegistry.h"

namespace dagbase
{
    template<typename T>
    struct MetaClassRegistration
    {
        MetaClassRegistration(const Atom& name)
            :
        name(name)
        {
            Type& type = T::getType();

            TypeRegistry::getTypeRegistry().registerType(name, &type);
        }

        ~MetaClassRegistration()
        {
            TypeRegistry::getTypeRegistry().unregisterType(name);
        }
        Atom name;
    };

}
