//
// Created by Tony Horrobin on 14/05/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Dimension.h"

namespace dagbase
{
    struct DAGBASE_API Unit
    {
        const Dimension* dimension{nullptr};
        double toSI{0.0};
        const char* symbol;
    };

    extern constexpr DAGBASE_API Unit METRE = {&LENGTH, 1.0, "m"};
    extern constexpr DAGBASE_API Unit YARD = {&LENGTH, 1.0/1.09361, "yd"};
    extern constexpr DAGBASE_API Unit KILOGRAM = { &MASS, 1.0, "kg"};
    extern constexpr DAGBASE_API Unit SECOND = {&TIME, 1.0, "s"};
    extern constexpr DAGBASE_API Unit HOUR={&TIME, 60.0, "h"};
    extern constexpr DAGBASE_API Unit METREPERSECOND{ &SPEED, 1.0, "ms^-1"};
    extern constexpr DAGBASE_API Unit MILEPERHOUR{ &SPEED, 1.0/2.2377, "mph"};
    extern constexpr DAGBASE_API Unit KILOMETERPERHOUR{ &SPEED, 1.0/3.6, "kph"};
    extern constexpr DAGBASE_API Unit METREPERSECONDSQUARED{&ACCELERATION, 1.0, "ms^-2"};
    extern constexpr DAGBASE_API Unit METREPERSECONDCUBED{&JERK, 1.0, "ms^-3"};
}
