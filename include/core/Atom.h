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

        static Atom intern(const std::string& name);

        Atom(const Atom& other)
        {
            _value = other._value;
        }

        bool operator==(const Atom& rhs) const
        {
            return _value == rhs._value;
        }

        bool operator!=(const Atom& rhs) const
        {
            return _value != rhs._value;
        }
    private:
        explicit Atom(const char* str);
        const char* _value{nullptr};
        using AtomMap = std::unordered_map<std::string, Atom>;
        static AtomMap _atoms;
    };
}