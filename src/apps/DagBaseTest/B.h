//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "../../../include/core/MetaClassRegistration.h"
#include "core/TypeRegistry.h"

#include <iostream>

class C;

class B
{
public:
    B() = default;

    static dagbase::Type& getType(bool complete=true);
    static dagbase::MetaClassRegistration<B> registration;
private:
    C* _c{nullptr};
};

