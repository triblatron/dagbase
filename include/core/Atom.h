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

        std::size_t length() const
        {
            return _length;
        }

        char operator[](std::size_t index) const
        {
            if (_value && index<_length)
            {
                 return _value[index];
            }

            return EOF;
        }

        Atom(const Atom& other)
        {
            _value = other._value;
            _length = other._length;
        }

        Atom& operator=(const Atom& rhs)
        {
            if (this!=&rhs)
            {
                _value = rhs._value;
                _length = rhs._length;
            }
            return *this;
        }

        bool operator==(const char* other) const
        {
            return _value && other && std::strcmp(_value, other) == 0;
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
        std::size_t _length{0};
        using AtomMap = std::unordered_map<std::string, Atom>;
        static AtomMap _atoms;
    };
}