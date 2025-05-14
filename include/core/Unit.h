//
// Created by Tony Horrobin on 14/05/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Dimension.h"

#include <cstring>

namespace dagbase
{
    struct DAGBASE_API Unit
    {
        const char* dimension{nullptr};
        double toSI{0.0};
        const char* symbol;

        bool operator==(const Unit& other) const
        {
            return strcmp(dimension,other.dimension)==0 &&
                toSI==other.toSI &&
                strcmp(symbol,other.symbol)==0;
        }

        static void parseQuantity(const char* str, double* value, Unit* unit);
    };

    extern constexpr DAGBASE_API Unit METRE = {Dimension::LENGTH, 1.0, "m"};
    extern constexpr DAGBASE_API Unit YARD = {Dimension::LENGTH, 1.0/1.09361, "yd"};
    extern constexpr DAGBASE_API Unit KILOGRAM = { Dimension::MASS, 1.0, "kg"};
    extern constexpr DAGBASE_API Unit SECOND = {Dimension::TIME, 1.0, "s"};
    extern constexpr DAGBASE_API Unit HOUR={Dimension::TIME, 60.0, "h"};
    extern constexpr DAGBASE_API Unit METREPERSECOND{ Dimension::SPEED, 1.0, "ms^-1"};
    extern constexpr DAGBASE_API Unit MILEPERHOUR{ Dimension::SPEED, 1.0/2.2377, "mph"};
    extern constexpr DAGBASE_API Unit KILOMETERPERHOUR{ Dimension::SPEED, 1.0/3.6, "kph"};
    extern constexpr DAGBASE_API Unit METREPERSECONDSQUARED{Dimension::ACCELERATION, 1.0, "ms^-2"};
    extern constexpr DAGBASE_API Unit METREPERSECONDCUBED{Dimension::JERK, 1.0, "ms^-3"};
}
