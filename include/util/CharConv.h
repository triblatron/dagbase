//
// Created by Tony Horrobin on 23/09/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include <cstdlib>

namespace dagbase
{
    //! Use the C version of numeric_limits<double>::max_digits10() to convert a double to a string using snprintf()
    //! \note This is a slow, snprintf()-based fallback if we find ourselves without to_chars() for double.
    //! This is true for APPLE platforms before macOS 26.0 for some reason, even though the overload is in C++17.
    void DAGBASE_API convertToChar(char* first, char* last, double value);
}
