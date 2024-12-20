#pragma once

#include "config/DagBaseExport.h"

#include <cstdarg>
#include <cstdio>
#include <typeinfo>
#include <sstream>

namespace dagbase
{
    struct DAGBASE_API ErrorDescriptor
    {
        int code;
        const char* name;
        const char* description;
    };
}
