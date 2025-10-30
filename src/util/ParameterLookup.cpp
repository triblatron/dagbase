//
// Created by Tony Horrobin on 30/10/2025.
//

#include "config/config.h"

#include "util/ParameterLookup.h"

namespace dagbase
{

    void ParameterLookup::configure(ConfigurationElement &config)
    {

    }

    Atom ParameterLookup::interpolate(const Atom &atom)
    {
        return atom;
    }
}