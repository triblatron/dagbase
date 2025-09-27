//
// Created by Tony Horrobin on 27/04/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Colour.h"

#include <string>
#include <string_view>

namespace dagbase
{
    class ConfigurationElement;

    struct DAGBASE_API Colour
    {
        float r{0.0f};
        float g{0.0f};
        float b{0.0f};
        float a{0.0f};

        void configure(ConfigurationElement& config);

        bool operator==(const Colour& other) const
        {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }

        bool operator!=(const Colour& other) const
        {
            return !(*this == other);
        }

        bool operator<(const Colour& other) const
        {
            return r < other.r || (r==other.r && g<other.g) || (g==other.g && b<other.b) || (b==other.b && a<other.a);
        }

        bool operator<=(const Colour& other) const
        {
            return *this < other || *this == other;
        }

        bool operator>(const Colour& other) const
        {
            return r > other.r || (r==other.r && g>other.g) || (g==other.g && b>other.b) || (b==other.b && a>other.a);
        }

        bool operator>=(const Colour& other) const
        {
            return *this > other || *this == other;
        }
    };

    std::string to_string(const Colour& colour);
}
