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

namespace dagbase
{
    class ConfigurationElement;

    class DAGBASE_API Atom
    {
    public:
        using const_iterator = const char*;
    public:
        Atom() = default;

        ~Atom();

        static Atom& intern(const std::string& name);

        void configure(ConfigurationElement& config);

        const char* value() const
        {
            return _value;
        }

        std::size_t length() const
        {
            return _length;
        }

        bool empty() const
        {
            return _length==0;
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

        void destroy()
        {
            if (_value)
                std::free((void*)_value);
        }

        static void clear()
        {
            std::for_each(_atoms.begin(), _atoms.end(), [](AtomMap::value_type& value) {
               value.second.destroy();
            });
            _atoms.clear();
        }

        const_iterator begin() const
        {
            return _value;
        }

        const_iterator end() const
        {
            return _value+_length;
        }

        std::string toString() const;
    private:
        explicit Atom(const char* str);
        const char* _value{nullptr};
        std::size_t _length{0};
        using AtomMap = std::unordered_map<std::string, Atom>;
        static AtomMap _atoms;
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
