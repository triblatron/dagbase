//
// Created by Tony Horrobin on 23/09/2026.
//

#include "config/config.h"

#include "util/CharConv.h"

#include <cstdio>
#include <cfloat>
#include <cstddef>

//! Generate the stringization of its input
#define TO_CHARS_FORMAT_HELPER(x) #x
//! Generate a format string for use with snprintf() et al
//! \note Use the double-layer stringizing trick to get token after expansion
//! Without this, the string value of the token itself will be generated
#define TO_CHARS_FORMAT(n) "%." TO_CHARS_FORMAT_HELPER(n) "g"

namespace dagbase
{
    // Fallback snprintf-based implementation of conversion of a double to a char array.
    void convertToChar(char* first, char* last, double value)
    {
        const char* fmt = TO_CHARS_FORMAT(DBL_DECIMAL_DIG);
        // Get the number of characters that would be written, not including the terminating nul,
        auto numRequired = std::snprintf(nullptr, 0, fmt, value);
        // Get number of bytes available, safe assuming that last and first are part of the same array.
        ptrdiff_t available = last - first;
        if (numRequired+1 <= available)
        {
            snprintf(first, numRequired+1, fmt, value);
        }
    }
}
