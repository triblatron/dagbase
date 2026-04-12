//
// Created by Tony Horrobin on 12/04/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include <cstdint>

namespace dagbase
{
    struct BufferUInt16
    {
        std::uint8_t bytes[sizeof(std::uint16_t)];
    };

    struct BufferInt16
    {
        std::uint8_t bytes[sizeof(std::int16_t)];
    };

    struct BufferUInt32
    {
        std::uint8_t bytes[sizeof(std::uint32_t)];
    };

    struct BufferInt32
    {
        std::uint8_t bytes[sizeof(std::int32_t)];
    };

    struct BufferFloat
    {
        std::uint8_t bytes[sizeof(float)];
    };

    struct BufferDouble
    {
        std::uint8_t bytes[sizeof(double)];
    };

    struct BufferUInt64
    {
        std::uint8_t bytes[sizeof(std::uint64_t)];
    };

    struct BufferInt64
    {
        std::uint8_t bytes[sizeof(std::int64_t)];
    };

}