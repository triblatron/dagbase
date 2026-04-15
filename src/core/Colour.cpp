//
// Created by Tony Horrobin on 27/04/2025.
//

#include "config/config.h"

#include "core/Colour.h"
#include "core/ConfigurationElement.h"
#include "util/Searchable.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"

namespace dagbase
{

    std::string to_string(const Colour &colour)
    {
        return "Colour { " + std::to_string(colour.r) + ", " + std::to_string(colour.g) + ", " + std::to_string(colour.b) + ", " + std::to_string(colour.a) + " }";
    }

    void Colour::configure(ConfigurationElement &config)
    {
        if (config.numChildren() == 4)
        {
            r = float(config.child(0)->asDouble());
            g = float(config.child(1)->asDouble());
            b = float(config.child(2)->asDouble());
            a = float(config.child(3)->asDouble());
        }
    }

    OutputStream & Colour::write(OutputStream &str) const
    {
        str.writeFloat(r);
        str.writeFloat(g);
        str.writeFloat(b);
        str.writeFloat(a);

        return str;
    }

    InputStream & Colour::read(InputStream &str)
    {
        str.readFloat(&r);
        str.readFloat(&g);
        str.readFloat(&b);
        str.readFloat(&a);

        return str;
    }

    /*
    Variant Colour::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "r", r);
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "g", g);
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "b", b);
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "a", a);
        if (retval.has_value())
            return retval;

        return {};
    }
*/
}
