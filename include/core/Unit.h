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

        enum WrapPolicy : std::uint32_t
        {
            //! Leave the out-of-bounds value alone
            WRAP_NONE,
            //! Clamp to within bounds
            WRAP_SATURATE,
            //! Use modulo arithmetic to generate a value in range
            WRAP_CYCLE,
            //! Set the value to zero.
            WRAP_DISCARD
        };

        enum ConversionResult : std::uint32_t
        {
            CONV_OK,
            CONV_NO_OUTPUT,
            CONV_INCOMPATIBLE_DIMS
        };

        const char* dimension{ nullptr };
        double toSI{ 0.0 };
        const char* symbol{ nullptr };
        double minValue{ -std::numeric_limits<double>::infinity() };
        double maxValue={ std::numeric_limits<double>::infinity() };
        WrapPolicy wrapPolicy{WRAP_NONE};

        bool operator==(const Unit& other) const
        {
            return dimension && other.dimension && std::strcmp(dimension,other.dimension)==0 &&
                toSI==other.toSI &&
                symbol && other.symbol && strcmp(symbol,other.symbol)==0;
        }

        double wrap(double value) const;

        static const Unit NONE;//{ Dimension::NONE, 1.0, "" };
        static const Unit METRE;// = { Dimension::LENGTH, 1.0, "m" };
        static const Unit YARD;// = { Dimension::LENGTH, 1.0 / 1.09361, "yd" };
        static const Unit MILE;// = { Dimension::LENGTH, 1609.344, "mi" };
        static const Unit KILOMETRE;// = { Dimension::LENGTH, 1000.0, "km" };
        static const Unit KILOGRAM;// = { Dimension::MASS, 1.0, "kg" };
        static const Unit SECOND;// = { Dimension::TIME, 1.0, "s" };
        static const Unit HOUR;// = { Dimension::TIME, 60.0, "h" };
        static const Unit METREPERSECOND;// { Dimension::SPEED, 1.0, "ms^-1" };
        static const Unit MILEPERHOUR;// { Dimension::SPEED, 1.0 / 2.23693629, "mih^-1" };
        static const Unit KILOMETREPERHOUR;// { Dimension::SPEED, 1.0 / 3.6, "kmh^-1" };
        static const Unit METREPERSECONDSQUARED;// { Dimension::ACCELERATION, 1.0, "ms^-2" };
        static const Unit METREPERSECONDCUBED;// { Dimension::JERK, 1.0, "ms^-3" };
        static const Unit PIXEL;// { Dimension::LENGTH, 1.0, "px" };
        static const Unit PERCENT;
        static const Unit NEWTON;
        static const Unit RADIAN;

        static void parseQuantity(const char* str, double* value, Unit* unit);

        static void parseRange(const char* str, double* minValue, double* maxValue, Unit* unit);

        static void parseUnit(const char* str, Unit* unit);

        static ConversionResult convert(double sourceValue, Unit sourceUnit, Unit destUnit, double* destValue);
    private:
        static std::map<std::string_view, Unit> allUnits;

        struct RegisterUnits
        {
            RegisterUnits();
        };

        static RegisterUnits registration;
    };
}
