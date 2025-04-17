//
// Created by Tony Horrobin on 15/04/2025.
//

#pragma once

#include <string>

#include "config/DagBaseExport.h"

#include <unordered_map>

namespace dagbase
{
    class DAGBASE_API Atom
    {
    public:
        Atom() = default;

        static Atom intern(std::string name);

        Atom(const Atom& other)
        {
            _value = other._value;
        }

        Atom& operator=(const Atom& rhs)
        {
            if (this!=&rhs)
            {
                _value = rhs._value;
            }
            return *this;
        }
        bool operator==(const Atom& rhs) const
        {
            return _value == rhs._value;
        }

        bool operator!=(const Atom& rhs) const
        {
            return _value != rhs._value;
        }

        bool operator<(const Atom& rhs) const
        {
            if (_value && rhs._value)
                return std::strcmp(_value,rhs._value)<0;
            return false;
        }

        static void reset()
        {
            _atoms.clear();
        }
    private:
        explicit Atom(const char* str);
        const char* _value{nullptr};
        using AtomMap = std::unordered_map<std::string, Atom>;
        static AtomMap _atoms;
    };
}