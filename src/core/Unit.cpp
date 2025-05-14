//
// Created by Tony Horrobin on 14/05/2025.
//

#include "config/config.h"

#include "core/Unit.h"

#include <cstdlib>
#include <cstring>

namespace dagbase
{
    void Unit::parseQuantity(const char *str, double *value, dagbase::Unit *unit)
    {
        if (value && unit)
        {
            char* endPtr=nullptr;
            *value = strtod(str, &endPtr);
            if (endPtr!=str)
            {
                if (strcmp(endPtr, METRE.symbol)==0)
                {
                    *unit = METRE;
                }
                else if (strcmp(endPtr, SECOND.symbol)==0)
                {
                    *unit = SECOND;
                }
            }
        }
    }
}