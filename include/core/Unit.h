//
// Created by Tony Horrobin on 14/05/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Dimension.h"

#include <cstring>
#include <map>
#include <string_view>

namespace dagbase
{
    struct DAGBASE_API Unit
    {
        enum ParseState : std::uint32_t
        {
            PARSE_INITIAL,
            PARSE_SYMBOL,
            PARSE_POWER_RAISE,
            PARSE_POWER_SIGN,
            PARSE_POWER_VALUE
        };

        enum ConversionResult : std::uint32_t
        {
            CONV_OK,
            CONV_NO_OUTPUT,
            CONV_INCOMPATIBLE_DIMS
        };

        const char* dimension{nullptr};
        double toSI{0.0};
        const char* symbol;

        bool operator==(const Unit& other) const
        {
            return dimension && other.dimension && std::strcmp(dimension,other.dimension)==0 &&
                toSI==other.toSI &&
                symbol && other.symbol && strcmp(symbol,other.symbol)==0;
        }

        static void parseQuantity(const char* str, double* value, Unit* unit);

        static ConversionResult convert(double sourceValue, Unit sourceUnit, Unit destUnit, double* destValue);

        static std::map<std::string_view, Unit> allUnits;

        struct RegisterUnits
        {
            RegisterUnits();
        };

        static RegisterUnits registration;
    };

    extern constexpr DAGBASE_API Unit NONE = {Dimension::NONE, 1.0, ""};
    extern constexpr DAGBASE_API Unit METRE = {Dimension::LENGTH, 1.0, "m"};
    extern constexpr DAGBASE_API Unit YARD = {Dimension::LENGTH, 1.0/1.09361, "yd"};
    extern constexpr DAGBASE_API Unit MILE= {Dimension::LENGTH, 1609.344, "mi"};
    extern constexpr DAGBASE_API Unit KILOMETRE = {Dimension::LENGTH, 1000.0, "km"};
    extern constexpr DAGBASE_API Unit KILOGRAM = { Dimension::MASS, 1.0, "kg"};
    extern constexpr DAGBASE_API Unit SECOND = {Dimension::TIME, 1.0, "s"};
    extern constexpr DAGBASE_API Unit HOUR={Dimension::TIME, 60.0, "h"};
    extern constexpr DAGBASE_API Unit METREPERSECOND{ Dimension::SPEED, 1.0, "ms^-1"};
    extern constexpr DAGBASE_API Unit MILEPERHOUR{ Dimension::SPEED, 1.0/2.23693629, "mih^-1"};
    extern constexpr DAGBASE_API Unit KILOMETREPERHOUR{Dimension::SPEED, 1.0 / 3.6, "kmh^-1"};
    extern constexpr DAGBASE_API Unit METREPERSECONDSQUARED{Dimension::ACCELERATION, 1.0, "ms^-2"};
    extern constexpr DAGBASE_API Unit METREPERSECONDCUBED{Dimension::JERK, 1.0, "ms^-3"};
    extern constexpr DAGBASE_API Unit PIXEL{ Dimension::LENGTH, 1.0, "px"};


}
