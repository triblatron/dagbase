//
// Created by Tony Horrobin on 28/04/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include <string>

namespace dagbase
{
    class OutputStream;
    class InputStream;

    struct DAGBASE_API Vec2
    {
        float x{0.0f};
        float y{0.0f};

        bool operator==(const Vec2& other) const
        {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Vec2& other) const
        {
            return !(*this == other);
        }

        bool operator<(const Vec2& other) const
        {
            return x < other.x || (x==other.x && y<other.y);
        }

        bool operator<=(const Vec2& other) const
        {
            return *this < other || *this == other;
        }

        bool operator>(const Vec2& other) const
        {
            return x > other.x || (x==other.x && y>other.y);
        }

        bool operator>=(const Vec2& other) const
        {
            return *this > other || *this == other;
        }

        OutputStream& write(OutputStream& str) const;

        InputStream& read(InputStream& str);

        static bool parse(std::string_view str, Vec2* value);
    };

    std::string to_string(const Vec2& vec);
}
