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
        allUnits.emplace("kph", KILOMETERPERHOUR);
        allUnits.emplace("m/s", METREPERSECOND);
        allUnits.emplace("m/s/s",METREPERSECONDSQUARED);
        allUnits.emplace("m/s/s/s",METREPERSECONDCUBED);
        allUnits.emplace(KILOGRAM.symbol, KILOGRAM);
        allUnits.emplace(KILOMETERPERHOUR.symbol, KILOMETERPERHOUR);
        allUnits.emplace(PIXEL.symbol, PIXEL);
    }
}
