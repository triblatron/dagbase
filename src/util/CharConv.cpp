//
// Created by Tony Horrobin on 23/09/2026.
//

#include "config/config.h"

#include "util/CharConv.h"

#include <limits>
#include <climits>
#include <cstdio>
#define FORMAT(n) "%." #n "g"

namespace dagbase
{
    char* convertToChar(double value, std::size_t* bufLen)
    {
        const char* fmt = FORMAT(17);
        // Get the number of characters that would be written.
        auto numRequired = std::snprintf(nullptr, 0, fmt, value);
        // Allocate buffer, accounting for nul terminator.
        auto buf = new char[numRequired + 1];
        std::snprintf(buf, numRequired+1, fmt, value);
        if (bufLen)
            *bufLen = numRequired+1;
        return buf;
    }
}
