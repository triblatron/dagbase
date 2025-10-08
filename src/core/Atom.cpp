//
// Created by Tony Horrobin on 15/04/2025.
//

#include "config/config.h"

#include "core/Atom.h"
#include "core/ConfigurationElement.h"

namespace dagbase
{
    Atom::AtomMap Atom::_atoms;

    Atom& Atom::intern(std::string name)
    {
        auto it = _atoms.find(name);
        if (it == _atoms.end())
        {
            return _atoms[name] = Atom(strdup(name.c_str()));
        }
        else
        {
            return it->second;
        }
    }

    Atom::Atom(const char* str)
        :
    _value(str)
    {
        if (_value)
        {
            _length = std::strlen(_value);
        }
    }

    Atom::~Atom()
    {
    }

    void Atom::configure(ConfigurationElement &config)
    {
        auto name = config.asString();
        auto it = _atoms.find(name);

        if (it == _atoms.end())
        {
            _value = strdup(name.c_str());
        }
        else
        {
            _value = it->second._value;
        }
    }
}
