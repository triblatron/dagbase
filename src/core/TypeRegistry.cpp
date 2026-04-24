//
// Created by Tony Horrobin on 20/04/2026.
//

#include "core/TypeRegistry.h"

#include <cstdint>
#include <string>

namespace dagbase
{
    Variant Type::find(std::string_view path) const
    {
        Variant retval;


        return {};
    }

    void TypeRegistry::registerType(dagbase::Atom name, Type* type)
    {
        _types.insert(dagbase::VectorMap<dagbase::Atom, Type*>::value_type(name, type));
    }

    void TypeRegistry::unregisterType(dagbase::Atom name)
    {
        if (auto it=_types.find(name); it!=_types.end())
            _types.erase(it);
    }

    Type * TypeRegistry::findType(dagbase::Atom name)
    {
        if (auto it=_types.find(name); it!=_types.end())
            return it->second;

        return nullptr;
    }
}