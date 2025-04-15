//
// Created by Tony Horrobin on 15/04/2025.
//

#include "config/config.h"

#include "core/Atom.h"

namespace dagbase
{
    Atom::AtomMap Atom::_atoms;

    Atom Atom::intern(const std::string& name)
    {
        auto it = _atoms.find(name);
        if (it == _atoms.end())
        {
            Atom atom(strdup(name.c_str()));
            _atoms[name] = atom;
            return atom;
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
        // Do nothing.
    }
}
