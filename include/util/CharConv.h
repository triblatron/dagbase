//
// Created by Tony Horrobin on 23/09/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include <cstdlib>

namespace dagbase
{
    //! Use the max_digits10 to convert a double to a string
    char DAGBASE_API * convertToChar(double value, std::size_t* bufLen);
}
