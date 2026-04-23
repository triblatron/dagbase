//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "core/TypeRegistry.h"

namespace dagbase
{
    struct Type;

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
#define DAGBASE_REGISTER_PRIMITIVE(name, primitive)                                                 \
    struct name                                                                                         \
    {                                                                                                   \
        static Type& getType();                                                                         \
        static dagbase::MetaClassRegistration<name> registration;                                       \
    };                                                                                                  \
    inline Type& name::getType()                                                                        \
    {                                                                                                   \
        static Type type;                                                                               \
        type.size = sizeof(primitive);                                                                  \
        type.complete = true;                                                                           \
        return type;                                                                                    \
    }                                                                                                   \
inline dagbase::MetaClassRegistration<name> name::registration(dagbase::Atom::intern(#name));


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
}
