//
// Created by Tony Horrobin on 14/05/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Dimension.h"

#include <cstring>
#include <map>
#include <string_view>
#include <limits>
#include <iosfwd>

namespace dagbase
{
    struct DAGBASE_API Unit
    {
        enum WrapPolicy : std::uint32_t
        {
            WRAP_UNKNOWN,
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
        // Scale(0), bias(1) to convert to SI units
        double toSI[2]{};
        const char* symbol{ nullptr };
        double minValue{ -std::numeric_limits<double>::infinity() };
        double maxValue={ std::numeric_limits<double>::infinity() };
        WrapPolicy wrapPolicy{WRAP_NONE};

        bool operator==(const Unit& other) const
        {
            return dimension && other.dimension && std::strcmp(dimension,other.dimension)==0 &&
                toSI[0]==other.toSI[0] && toSI[1]==other.toSI[1] &&
                symbol && other.symbol &&
                    strcmp(symbol,other.symbol)==0 &&
                        minValue==other.minValue &&
                            maxValue==other.maxValue &&
                                wrapPolicy==other.wrapPolicy;
        }

        double wrap(double value) const;

        static const Unit NONE;
        static const Unit METRE;
        static const Unit YARD;
        static const Unit MILE;
        static const Unit KILOMETRE;
        static const Unit KILOGRAM;
        static const Unit SECOND;
        static const Unit MINUTE;
        static const Unit HOUR;
        static const Unit METREPERSECOND;
        static const Unit MILEPERHOUR;
        static const Unit KILOMETREPERHOUR;
        static const Unit METREPERSECONDSQUARED;
        static const Unit METREPERSECONDCUBED;
        static const Unit PIXEL;
        static const Unit PERCENT;
        static const Unit NEWTON;
        static const Unit RADIAN;
        static const Unit DEGREE;
        static const Unit KELVIN;
        static const Unit CELSIUS;
        static const Unit FAHRENHEIT;
        static const Unit AMPERE;
        static const Unit MOLE;
        static const Unit CANDELA;

        static void parseQuantity(const char* str, double* value, Unit* unit);

        static void parseRange(const char* str, double* minValue, double* maxValue, Unit* unit);

        static void parseUnit(const char* str, Unit* unit);

        static ConversionResult convert(double sourceValue, Unit sourceUnit, Unit destUnit, double* destValue);

        static const char* wrapPolicyToString(WrapPolicy value);

        static WrapPolicy parseWrapPolicy(const char* str);
    private:
        static std::map<std::string_view, Unit> allUnits;

        struct RegisterUnits
        {
            RegisterUnits();
        };

        static RegisterUnits registration;
    };

    std::ostream DAGBASE_API & operator<<(std::ostream& str, const Unit& value);
}
