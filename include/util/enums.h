//
// Created by tony on 20/08/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include <cstring>

#define BIT_NAME(mask, bit, str) \
    if (mask & bit) \
    {                            \
        str += #bit;             \
        str += ' ';              \
    }

#define ENUM_NAME(name) \
	case (name): return #name;
	
#define TEST_BIT(needle, haystack, mask) \
    if (haystack.find(#needle)!=std::string::npos) \
    {                              \
        mask = static_cast<decltype(mask)>(mask | needle);            \
    }
    

#define TEST_ENUM(needle, haystack) \
	if (std::strcmp(#needle, haystack)==0)\
	{\
		return needle;\
	}

