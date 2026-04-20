//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "core/TypeRegistry.h"
#include "MetaClassRegistration.h"

class B;

class C
{
public:
    C() = default;

    static dagbase::Type& getType(bool complete=true);
    static MetaClassRegistration<C> registration;
private:
    B* _b{nullptr};
};
