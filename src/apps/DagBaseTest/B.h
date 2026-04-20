//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "MetaClassRegistration.h"
#include "core/TypeRegistry.h"

#include <iostream>

class C;

class B
{
public:
    B() = default;

    static dagbase::Type& getType(bool complete=true);
    static MetaClassRegistration<B> registration;
private:
    C* _c{nullptr};
};

template<>
inline MetaClassRegistration<B>::MetaClassRegistration()
{
    dagbase::Type& type = B::getType();
    dagbase::TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("B"), &type);
}

template<>
inline MetaClassRegistration<B>::~MetaClassRegistration()
{
    std::cout << "MetaClassRegistration<B> unregistering" << std::endl;
    dagbase::TypeRegistry::getTypeRegistry().unregisterType(dagbase::Atom::intern("B"));
}

