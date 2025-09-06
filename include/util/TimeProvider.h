//
// Created by Tony Horrobin on 06/09/2025.
//

#pragma once

#include "config/DagBaseExport.h"

namespace dagbase
{
    class DAGBASE_API TimeProvider
    {
    public:
        virtual ~TimeProvider() = default;

        virtual void tick() = 0;

        virtual double provideTime() const = 0;
    };
}