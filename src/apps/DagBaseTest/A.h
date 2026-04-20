//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "MetaClassRegistration.h"
#include "core/TypeRegistry.h"
#include <iostream>

class B;

class A
{
public:
    A() = default;

    static dagbase::Type& getType();

    static MetaClassRegistration<A> registration;
private:
    A* _parent{nullptr};
};

template<>
inline MetaClassRegistration<A>::MetaClassRegistration()
{
    dagbase::Type& type = A::getType();
    dagbase::TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("A"), &type);
}

template<>
inline MetaClassRegistration<A>::~MetaClassRegistration()
{
    std::cout << "MetaClassRegistration<A> unregistering" << std::endl;
    dagbase::TypeRegistry::getTypeRegistry().unregisterType(dagbase::Atom::intern("A"));
}
