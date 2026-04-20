//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "core/TypeRegistry.h"
#include "../../../include/core/MetaClassRegistration.h"

class B;

class C
{
public:
    C() = default;

    static dagbase::Type& getType(bool complete=true);
    static dagbase::MetaClassRegistration<C> registration;
private:
    B* _b{nullptr};
};
