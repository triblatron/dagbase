//
// Created by Tony Horrobin on 20/04/2026.
//

#include "core/TypeRegistry.h"
#include "core/MetaClassRegistration.h"

#include <cstdint>
#include <string>

namespace dagbase
{
    DAGBASE_REGISTER_PRIMITIVE(Int8, std::int8_t)
    DAGBASE_REGISTER_PRIMITIVE(UInt8, std::uint8_t)
    DAGBASE_REGISTER_PRIMITIVE(Int16, std::int16_t)
    DAGBASE_REGISTER_PRIMITIVE(UInt16, std::uint16_t)
    DAGBASE_REGISTER_PRIMITIVE(Int32, std::int32_t)
    DAGBASE_REGISTER_PRIMITIVE(UInt32, std::uint32_t)
    DAGBASE_REGISTER_PRIMITIVE(Int64, std::int64_t)
    DAGBASE_REGISTER_PRIMITIVE(UInt64, std::uint64_t)
    DAGBASE_REGISTER_PRIMITIVE(Float, float)
    DAGBASE_REGISTER_PRIMITIVE(Double, double)
    DAGBASE_REGISTER_PRIMITIVE(String, std::string)
    DAGBASE_REGISTER_PRIMITIVE(WString, std::wstring)
    DAGBASE_REGISTER_PRIMITIVE(Boolean, bool)

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