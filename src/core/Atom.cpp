//
// Created by Tony Horrobin on 15/04/2025.
//

#include "config/config.h"

#include "core/Atom.h"
#include "core/ConfigurationElement.h"

#include <algorithm>

namespace dagbase
{
    Atom::AtomMap Atom::_atoms;

    Atom& Atom::intern(const std::string& name)
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

    Atom::Atom(const char *str, std::size_t length)
        :
    _value(str),
    _length(length)
    {
        // Do nothing.
    }

    Atom::~Atom()
    {
    }

    std::size_t Atom::find(char needle) const
    {
        auto ptr = std::strchr(_value, needle);
        return ptr?ptr - _value: npos;
    }

    Atom Atom::substr(std::size_t index, std::size_t length) const
    {
        if (index<_length)
        {
            const char* ptr = _value+index;
            length = std::min(_length-index,length-index);
            return intern(std::string(ptr, length));
        }

        return {};
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

    std::string Atom::toString() const
    {
        if (_value)
            return (_value);
        else
            return {};
    }
}
