//
// Created by Tony Horrobin on 14/05/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include <cstdint>

namespace dagbase
{
    enum BaseDimension : std::uint32_t
    {
        DIM_LENGTH,
        DIM_MASS,
        DIM_TIME,
        DIM_CURRENT,
        DIM_TERPERATURE,
        DIM_AMOUNT,
        DIM_LUMINOUS_INTENSITY,
        NUM_BASE_DIMENSIONS
    };
}