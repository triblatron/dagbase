//
// Created by Tony Horrobin on 15/04/2025.
//

#include "config/config.h"

#include "core/Atom.h"
#include "core/ConfigurationElement.h"

#include <algorithm>

namespace dagbase
{
    Atom& Atom::intern(const std::string& name)
    {
        AtomMap& atoms = getAtoms();
        auto [it, inserted] = atoms.emplace(name, Atom{});
        if (inserted)
            it->second._value = &it->first;
        return it->second;
    }
/*
    Atom::Atom(const char* str)
        :
    _value(str)
    {
    }

    Atom::Atom(const char *str, std::size_t length)
        :
    (*_value(str)
    {
        // Do nothing.
    }
*/
    Atom::~Atom()
    {
    }

    std::size_t Atom::find(char needle) const
    {
        if (_value)
        {
            auto pos = _value->find(needle);

            return pos;
        }

        return npos;
    }

    Atom Atom::substr(std::string::difference_type index, std::size_t length) const
    {
        if (_value && std::size_t(index)<_value->length())
        {
            auto it = _value->begin()+index;
            length = std::min(_value->length()-index,length-index);
            return intern(std::string(it, it+std::size_t(length)));
        }

        return {};
    }

    void Atom::configure(ConfigurationElement &config)
    {
        auto name = config.asString();

        intern(name);
    }

    Atom::AtomMap & Atom::getAtoms()
    {
        static AtomMap atoms;

        return atoms;
    }

    std::string Atom::toString() const
    {
        if (_value)
            return (*_value);
        else
            return {};
    }
}
