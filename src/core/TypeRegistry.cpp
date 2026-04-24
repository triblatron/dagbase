//
// Created by Tony Horrobin on 20/04/2026.
//

#include "core/TypeRegistry.h"

#include <cstdint>
#include <string>

namespace dagbase
{
    void TypeRegistry::registerType(Atom name, Type* type)
    {
        _types.insert(VectorMap<Atom, Type*>::value_type(name, type));
    }

    void TypeRegistry::unregisterType(Atom name)
    {
        if (auto it=_types.find(name); it!=_types.end())
            _types.erase(it);
    }

    Type * TypeRegistry::findType(Atom name)
    {
        if (auto it=_types.find(name); it!=_types.end())
            return it->second;

        return nullptr;
    }
}