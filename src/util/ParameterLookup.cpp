//
// Created by Tony Horrobin on 30/10/2025.
//

#include "config/config.h"

#include "util/ParameterLookup.h"
#include "core/ConfigurationElement.h"

#include <cctype>

namespace dagbase
{

    void ParameterLookup::configure(ConfigurationElement &config)
    {
        config.eachChild([this](ConfigurationElement& child) {
            _lookup.emplace(Atom::intern(child.name()), child.value());
            return true;
        });
    }

    Atom ParameterLookup::interpolate(const Atom &atom)
    {
        State state{STATE_INITIAL};
        std::string output;
        std::string name;
        for (auto c : atom)
        {
            switch (state)
            {
                case STATE_INITIAL:
                    if (std::isalnum(c))
                    {
                        output += c;
                    }
                    else if (c == '{')
                    {
                        name.clear();
                        state = STATE_FOUND_OPEN;
                    }
                    break;
                case STATE_FOUND_OPEN:
                    if (c != '}')
                    {
                        name += c;
                    }
                    else
                    {
                        if (auto it=_lookup.find(Atom::intern(name)); it!=_lookup.end())
                        {
                            output += it->second.toString();
                        }
                        state = STATE_INITIAL;
                    }
                    break;
            }
        }
        return Atom::intern(output);
    }
}