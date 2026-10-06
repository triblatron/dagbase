//
// Created by Tony Horrobin on 06/10/2026.
//


#include "config/config.h"

#include "util/EnumValue.h"
#include "core/TypeRegistry.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"

namespace dagbase
{
    EnumValue::EnumValue(Type *type):
        _type(type)
    {
        if (_type)
        {
            _value = _type->unknownValue;
        }
    }

    void EnumValue::set(const char *str)
    {
        if (_type && _type->parse)
        {
            _value = _type->parse(str);
        }
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
        str.writeUInt32(_value);

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

        str.readUInt32(&_value);

        return str;
    }
}
