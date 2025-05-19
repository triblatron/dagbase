//
// Created by Tony Horrobin on 28/04/2025.
//

#include "config/config.h"

#include "core/Vec2.h"
#include <sstream>

namespace dagbase
{
    std::string to_string(const Vec2& vec)
    {
        return "Vec2 { " + std::to_string(vec.x) + ", " + std::to_string(vec.y) + " }";
    }

    bool Vec2::parse(std::string_view str, Vec2* value)
    {
        if (!value)
            return false;

        std::istringstream istr(str.data());

        std::string temp;
        istr >> temp;
        if (temp != "Vec2")
        {
            return false;
        }
        istr >> temp;
        if (temp != "{")
        {
            return false;
        }

        istr >> value->x;

        istr >> value->y;
        istr >> temp;
        if (temp !="}")
            return false;

        return true;
    }
}