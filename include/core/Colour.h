//
// Created by Tony Horrobin on 27/04/2025.
//

#include "config/DagBaseExport.h"

#include <string>

namespace dagbase
{
    struct DAGBASE_API Colour
    {
        float r{0.0f};
        float g{0.0f};
        float b{0.0f};
        float a{0.0f};

        bool operator==(const Colour& other) const
        {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }
    };

    std::string to_string(const Colour& colour);
}
