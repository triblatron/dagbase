//
// Created by Tony Horrobin on 28/04/2025.
//

#include "config/config.h"

#include "core/Vec2.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"

#include <sstream>

namespace dagbase
{
    std::string to_string(const Vec2& vec)
    {
        return "Vec2 { " + std::to_string(vec.x) + ", " + std::to_string(vec.y) + " }";
    }

    OutputStream & Vec2::write(OutputStream &str) const
    {
        str.writeFloat(x);
        str.writeFloat(y);

        return str;
    }

    InputStream & Vec2::read(InputStream &str)
    {
        str.readFloat(&x);
        str.readFloat(&y);

        return str;
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