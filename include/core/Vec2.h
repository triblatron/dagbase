//
// Created by Tony Horrobin on 28/04/2025.
//

#include "config/DagBaseExport.h"

#include <string>

namespace dagbase
{
    struct DAGBASE_API Vec2
    {
        float x{0.0f};
        float y{0.0f};

        bool operator==(const Vec2& other) const
        {
            return x == other.x && y == other.y;
        }
    };

    std::string to_string(const Vec2& vec);
}
