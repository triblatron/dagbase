//
// Created by Tony Horrobin on 28/04/2025.
//

#include "config/config.h"

#include "core/Vec2.h"

namespace dagbase
{
    std::string to_string(const Vec2& vec)
    {
        return "Vec2 { " + std::to_string(vec.x) + ", " + std::to_string(vec.y) + " }";
    }
}