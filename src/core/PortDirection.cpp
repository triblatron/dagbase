//
// Created by Tony Horrobin on 11/10/2026.
//

#include "config/config.h"

#include "core/PortDirection.h"
#include "util/enums.h"

#include <cstring>

namespace dagbase
{
    PortDirection::Direction PortDirection::parseFromString(const char* str)
    {
        TEST_ENUM(DIR_IN, str)
        TEST_ENUM(DIR_OUT, str)
        TEST_ENUM(DIR_INTERNAL, str)

        return DIR_UNKNOWN;
    }

    const char *PortDirection::toString(PortDirection::Direction dir)
    {
        switch (dir)
        {
            ENUM_NAME(DIR_UNKNOWN)
            ENUM_NAME(DIR_IN)
            ENUM_NAME(DIR_OUT)
            ENUM_NAME(DIR_INTERNAL)
        }

        return "<error>";
    }
}