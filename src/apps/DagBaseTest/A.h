//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "../../../include/core/MetaClassRegistration.h"
#include "core/TypeRegistry.h"
#include <iostream>

class B;

class A
{
public:
    A() = default;

    static dagbase::Type& getType();

    static dagbase::MetaClassRegistration<A> registration;
private:
    A* _parent{nullptr};
};
