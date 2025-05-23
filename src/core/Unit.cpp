//
// Created by Tony Horrobin on 14/05/2025.
//

#include "config/config.h"

#include "core/Unit.h"

#include <cstdlib>
#include <cstring>
#include <cctype>

namespace dagbase
{
    std::map<std::string_view,Unit> Unit::allUnits;

    const Unit Unit::NONE{ Dimension::NONE, 1.0, "" };
    const Unit Unit::METRE{ Dimension::LENGTH, 1.0, "m" };
    const Unit Unit::YARD{ Dimension::TIME, 1.0, "s" };
    const Unit Unit::MILE{ Dimension::LENGTH, 1609.344, "mi" };
    const Unit Unit::KILOMETRE{ Dimension::LENGTH, 1000.0, "km" };
    const Unit Unit::KILOGRAM{ Dimension::MASS, 1.0, "kg" };
    const Unit Unit::SECOND{ Dimension::TIME, 1.0, "s" };
    const Unit Unit::HOUR{ Dimension::TIME, 60.0, "h" };
    const Unit Unit::METREPERSECOND{ Dimension::SPEED, 1.0, "ms^-1" };
    const Unit Unit::MILEPERHOUR{ Dimension::SPEED, 1.0 / 2.23693629, "mih^-1" };
    const Unit Unit::KILOMETREPERHOUR{ Dimension::SPEED, 1.0 / 3.6, "kmh^-1" };
    const Unit Unit::METREPERSECONDSQUARED{ Dimension::ACCELERATION, 1.0, "ms^-2" };
    const Unit Unit::METREPERSECONDCUBED{ Dimension::JERK, 1.0, "ms^-3" };
    const Unit Unit::PIXEL{ Dimension::LENGTH, 1.0, "px" };

    void Unit::parseQuantity(const char *str, double *value, dagbase::Unit *unit)
    {
        if (value && unit)
        {
            ParseState state{PARSE_SYMBOL};
            char* endPtr=nullptr;
            *value = strtod(str, &endPtr);

            if (endPtr!=str)
            {
                while (std::isspace(*endPtr))
                    ++endPtr;
                std::string_view rest = endPtr;
                if (auto it=allUnits.find(rest); it!=allUnits.end())
                {
                    *unit = it->second;
                }
                else
                {
                    *unit = NONE;
                }
            }
            else
            {
                *unit = NONE;
            }

//                if (strcmp(endPtr, METRE.symbol)==0)
//                {
//                    *unit = METRE;
//                }
//                else if (strcmp(endPtr, SECOND.symbol)==0)
//                {
//                    *unit = SECOND;
//                }
//            }
        }
    }

    Unit::ConversionResult Unit::convert(double sourceValue, Unit sourceUnit, Unit destUnit, double *destValue)
    {
        if (destValue)
        {
            if (sourceUnit.dimension && destUnit.dimension && std::strcmp(sourceUnit.dimension, destUnit.dimension) == 0)
            {
                *destValue = sourceValue * sourceUnit.toSI / destUnit.toSI;
                return CONV_OK;
            }
            else
            {
                *destValue = 0.0;
                return CONV_INCOMPATIBLE_DIMS;
            }
        }

        return CONV_NO_OUTPUT;
    }

    Unit::RegisterUnits Unit::registration;

    Unit::RegisterUnits::RegisterUnits()
    {
        allUnits.emplace(METRE.symbol, METRE);
        allUnits.emplace(YARD.symbol, YARD);
        allUnits.emplace(MILE.symbol, MILE);
        allUnits.emplace(SECOND.symbol, SECOND);
        allUnits.emplace(HOUR.symbol, HOUR);
        allUnits.emplace(METREPERSECOND.symbol, METREPERSECOND);
        allUnits.emplace(METREPERSECONDSQUARED.symbol, METREPERSECONDSQUARED);
        allUnits.emplace(METREPERSECONDCUBED.symbol, METREPERSECONDCUBED);
        allUnits.emplace(MILEPERHOUR.symbol, MILEPERHOUR);
        allUnits.emplace("mph", MILEPERHOUR);
        allUnits.emplace("kph", KILOMETREPERHOUR);
        allUnits.emplace("m/s", METREPERSECOND);
        allUnits.emplace("m/s/s",METREPERSECONDSQUARED);
        allUnits.emplace("m/s/s/s",METREPERSECONDCUBED);
        allUnits.emplace(KILOGRAM.symbol, KILOGRAM);
        allUnits.emplace(KILOMETREPERHOUR.symbol, KILOMETREPERHOUR);
        allUnits.emplace(PIXEL.symbol, PIXEL);
    }
}
