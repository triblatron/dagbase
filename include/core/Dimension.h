//
// Created by Tony Horrobin on 14/05/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "BaseDimension.h"

namespace dagbase
{
    struct DAGBASE_API Dimension
    {
        //int powers[BaseDimension::NUM_BASE_DIMENSIONS] {};
        const char* symbol;
    };

    extern constexpr DAGBASE_API Dimension LENGTH{"L"};
    extern constexpr DAGBASE_API Dimension MASS{"M"};
    extern constexpr DAGBASE_API Dimension TIME{"T"};
    extern constexpr DAGBASE_API Dimension SPEED{"LT^-1"};
    extern constexpr DAGBASE_API Dimension ACCELERATION{ "LT^-2"};
    extern constexpr DAGBASE_API Dimension JERK{"LT^-3"};
}
