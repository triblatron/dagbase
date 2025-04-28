//
// Created by Tony Horrobin on 27/04/2025.
//

#include "config/config.h"

#include "core/Colour.h"

namespace dagbase
{

    std::string to_string(const Colour &colour)
    {
        return "Colour { " + std::to_string(colour.r) + ", " + std::to_string(colour.g) + ", " + std::to_string(colour.b) + ", " + std::to_string(colour.a) + " }";
    }
}
