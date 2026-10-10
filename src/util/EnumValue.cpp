//
// Created by Tony Horrobin on 06/10/2026.
//


#include "config/config.h"

#include "util/EnumValue.h"

#include "core/TypeRegistry.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"

#include <memory_resource>

namespace dagbase
{
    EnumValue::EnumValue(Type *type, const char* str):
        _type(type)
    {
        if (_type)
        {
            _value = _type->unknownValue;
        }
        if (str)
        {
            set(str);
        }
    }

    void EnumValue::set(const char *str)
    {
        if (_type && _type->parse)
        {
            _value = _type->parse(str);
        }
    }

    bool EnumValue::isSelected(int n) const
    {
        if (_type)
        {
            return _value - _type->minValue == n;
        }
        return false;
    }

    std::string EnumValue::toString() const
    {
        if (_type && _type->toString)
        {
            return _type->toString(_value);
        }

        return "<error>";
    }

    OutputStream & EnumValue::writeToStream(OutputStream &str) const
    {
        str.writeString(_type->name.toString(), false);
        char buf[sizeof(_value)+1]{};
        Value v(static_cast<std::uint64_t>(_value));
        std::pmr::monotonic_buffer_resource pool{buf, sizeof(buf)};
        std::pmr::vector<std::uint8_t> actual{&pool};
        v.varintEncode(7, &actual);
        str.writeVariableLengthInteger(actual.data(), actual.size());

        return str;
    }

    InputStream &EnumValue::readFromStream(InputStream &str)
    {
        std::string typeName;
        str.readString(&typeName, false);
        if (auto type = TypeRegistry::getTypeRegistry().findType(Atom::intern(typeName)); type)
        {
            _type = type;
        }
        std::uint64_t valueFromStream;
        str.readVariableLengthInteger(7, &valueFromStream);
        _value = valueFromStream;

        return str;
    }
}
