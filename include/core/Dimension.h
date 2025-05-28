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
        static constexpr const char* NONE{""};
        static constexpr const char* LENGTH{"L"};
        static constexpr const char* MASS{"M"};
        static constexpr const char* TIME{"T"};
        static constexpr const char* SPEED{"LT^-1"};
        static constexpr const char* ACCELERATION{ "LT^-2"};
        static constexpr const char* JERK{"LT^-3"};
        static constexpr const char* FORCE{"MLT^-2"};
    };

}
