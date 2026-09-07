#pragma once

#include "config/DagBaseExport.h"

#include "core/Types.h"

#include <cstdint>
#include <variant>
#include <string>
#include <vector>
#include <utility>

namespace dagbase
{
    //! A value type to be used throughout the software, uses std::variant.
    //! Provides useful conversions and constructors.
    class DAGBASE_API Value
    {
    public:
        //! Typedef to improve readability when referencing the type of the variant.
        typedef std::variant<std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t, std::int32_t, std::uint64_t, std::int64_t, float, double, std::string, bool, Vec2, void*, std::vector<Value >> ValueType;
        
    public:
        //! Default ctor, initialises variant member to its first choice, an int64_t.
        Value() = default;

        //! Increment operator for types that can be added.
        template<typename T>
        Value& operator+=(T const& op)
        {
            std::get<T>(_value) += op;
            
            return *this;
        }

        explicit Value(ValueType value)
	        :
        _value(std::move(value))
        {
	        // Do nothing.
        }
        //! A constraint for the supported types.
        //! Certain template methods and operators can only be instantiated for types that satisfy it.
        template<typename T>
        using EnableIfSupported = std::enable_if_t<
            std::is_convertible_v<T, std::string> || std::is_convertible_v<T, std::int64_t> || std::is_convertible_v<T, bool> || std::is_convertible_v<T, double> || std::is_convertible_v<T, void*> || std::is_convertible_v<T, Vec2> || std::is_convertible_v<T, std::vector<Value>>>;

        //! Perfect forwarding ctor for supported types.
        template<typename T,typename = EnableIfSupported<T>>
        explicit Value(T&& value)
            :
            _value(std::forward<T>(value))
        {
            // Do nothing.
        }

        //! Copy ctor.
        Value(const Value& other) = default;

        //! Move ctor.
        Value(Value&& value) = default;

        //! Assignment of another Value.
        Value& operator=(const Value& value)
        {
            if (this != &value)
            {
                _value = value._value;
            }

            return *this;
        }
        
        //! Perfect-forwarding assignment of supported types.
        template<typename T, typename = EnableIfSupported<T>>
        Value& operator=(T&& value)
        {
            _value = std::forward<T>(value);

            return *this;
        }

        //! Allow conversion to any type we can hold.
        //! Prevents throwing at runtime by causing a compile-time 
        //! error for unsupported types.
        template<typename T, typename = EnableIfSupported<T>>
        explicit operator T() const
        {
            return std::get<T>(_value);
        }

        bool operator==(const Value& other) const
        {
            return _value == other._value;
        }

        //! Convert to our variant, required for template ctor to compile.
        operator ValueType() const
        {
            return _value;
        }

        PortType::Type type() const
        {
            return static_cast<PortType::Type>(_value.index());
        }

        bool empty() const
        {
            return type() == PortType::TYPE_VECTOR?std::get<PortType::TYPE_VECTOR>(_value).empty():true;
        }

        Value operator[](std::size_t index)
        {
            if (type() == PortType::TYPE_VECTOR)
            {
                auto vec = std::get<PortType::TYPE_VECTOR>(_value);

                if (index < vec.size())
                {
                    return vec[index];
                }
            }

            return {};
        }

        //! Add a value, converting to an array type if necessary.
        //! \note The original value is put into the first element of the array.
        template<typename T>
        void push_back(T v)
        {
            // If we are currently a vector, append the value.
            if (_value.index()==PortType::TYPE_VECTOR)
            {
                std::get<std::vector<Value>>(_value).push_back(Value(v));
            }
            else
            // If we are currently a scalar, convert to a vector with the original scalar and the new value.
            {
                _value = std::vector<Value> {*this,Value(v)};
            }
        }
    private:
        ValueType _value;
    };
}
