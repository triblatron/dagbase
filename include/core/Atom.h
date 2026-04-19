//
// Created by Tony Horrobin on 15/04/2025.
//

#pragma once

#include <string>

#include "config/DagBaseExport.h"

#include <unordered_map>
#include <algorithm>
#include <cstring>
#include <string>
#include <cassert>

namespace dagbase
{
    class ConfigurationElement;

    class DAGBASE_API Atom
    {
    public:
        using const_iterator = std::string::const_iterator;
        static constexpr std::size_t npos = std::string::npos;
        using AtomMap = std::unordered_map<std::string, Atom>;
    public:
        Atom() = default;

        ~Atom();

        std::size_t find(char needle) const;

        Atom substr(std::string::difference_type index, std::size_t length=npos) const;

        static Atom& intern(const std::string& name);

        void configure(ConfigurationElement& config);

        const char* value() const
        {
            return _value->c_str();
        }

        std::size_t length() const
        {
            if (_value)
                return _value->length();

            return std::size_t{0};
        }

        bool empty() const
        {
            if (_value)
                return _value->empty();

            return true;
        }

        char operator[](std::size_t index) const
        {
            if (_value && index<_value->length())
            {
                 return (*_value)[index];
            }

            return EOF;
        }

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

        bool operator==(const char* other) const
        {
            return _value && other && (*_value) == other;
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
            return _value < rhs._value;
        }

        void destroy()
        {
        }

        static AtomMap& getAtoms();

        static void clear()
        {
            AtomMap& atoms = getAtoms();
            std::for_each(atoms.begin(), atoms.end(), [](AtomMap::value_type& value) {
               value.second.destroy();
            });
            atoms.clear();
        }

        const_iterator begin() const
        {
            return _value->begin();
        }

        const_iterator end() const
        {
            return _value->end();
        }

        std::string toString() const;
    private:
        explicit Atom(const char* str);
        explicit Atom(const char* str, std::size_t length);
        const std::string* _value{nullptr};
    };
}

namespace std
{
    template<>
    struct hash<dagbase::Atom>
    {
        size_t operator()(const dagbase::Atom& key) const
        {
            return hash<const char*>{}(key.value());
        }
    };
    template<>
    struct hash<const dagbase::Atom>
    {
        size_t operator()(const dagbase::Atom& key) const
        {
            return hash<const char*>{}(key.value());
        }
    };
}
