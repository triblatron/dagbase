//
// Created by Tony Horrobin on 09/09/2026.
//

#include "config/config.h"

#include "core/Value.h"
#include "core/Editable.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"

#include <cassert>

namespace dagbase
{
    bool Value::operator<(const Value &other) const
    {
        return _value < other._value;
    }

    void Value::edit(const char* label, ImGuiContext *context)
    {
        switch (type())
        {
            case TYPE_INT64:
                Editable::editType(label, &std::get<TYPE_INT64>(_value));
                break;
            case TYPE_STRING:
                Editable::editType(label, &std::get<TYPE_STRING>(_value));
                break;
            case TYPE_DOUBLE:
                Editable::editType(label, &std::get<TYPE_DOUBLE>(_value));
                break;
            case TYPE_BOOL:
                Editable::editType(label, &std::get<TYPE_BOOL>(_value));
                break;
            default:
                assert(false);
                break;
        }
    }

    OutputStream & Value::writeToStream(OutputStream &str) const
    {
        str.writeHeader("Value");
        str.writeField("type");
        str.writeUInt8(type());
        str.writeField("value");
        switch (type())
        {
            case TYPE_UINT8:
                str.writeUInt8(operator std::uint8_t());
                break;
            case TYPE_INT8:
                str.writeInt8(operator std::int8_t());
                break;
            case TYPE_UINT16:
                str.writeUInt16(operator std::uint16_t());
                break;
            case TYPE_INT16:
                str.writeInt16(operator std::int16_t());
                break;
            case TYPE_UINT32:
                str.writeUInt32(operator std::uint32_t());
                break;
            case TYPE_INT32:
                str.writeInt32(operator std::int32_t());
                break;
            case TYPE_UINT64:
                str.writeUInt64(operator std::uint64_t());
                break;
            case TYPE_INT64:
                str.writeInt64(operator std::int64_t());
                break;
            case TYPE_FLOAT:
                str.writeFloat(operator float());
                break;
            case TYPE_DOUBLE:
                str.writeDouble(operator double());
                break;
            case TYPE_STRING:
                str.writeString(operator std::string(), false);
                break;
            case TYPE_BOOL:
                str.writeBool(operator bool());
                break;
            case TYPE_VEC2:
                operator Vec2().write(str);
                break;
            case TYPE_OPAQUE:
                assert(false);
            case TYPE_VECTOR:
            {
                const auto & vec = operator std::vector<Value>();
                str.writeUInt32(vec.size());
                for (auto v : vec)
                {
                    v.writeToStream(str);
                }
                break;
            }
        }
        str.writeFooter();
        
        return str;
    }

    InputStream & Value::readFromStream(InputStream &str)
    {
        std::string className;
        std::string fieldName;
        str.readHeader(&className);
        str.readField(&fieldName);
        std::uint8_t typeFromStream{};
        str.readUInt8(&typeFromStream);
        auto actualType = Type(typeFromStream);
        str.readField(&fieldName);
        switch (actualType)
        {
            case TYPE_UINT8:
            {
                std::uint8_t valueFromStream{};
                str.readUInt8(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_INT8:
            {
                std::int8_t valueFromStream{};
                str.readInt8(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_UINT16:
            {
                std::uint16_t valueFromStream{};
                str.readUInt16(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_INT16:
            {
                std::int16_t valueFromStream{};
                str.readInt16(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_UINT32:
            {
                std::uint32_t valueFromStream{};
                str.readUInt32(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_INT32:
            {
                std::int32_t valueFromStream{};
                str.readInt32(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_UINT64:
            {
                std::uint64_t valueFromStream{};
                str.readUInt64(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_INT64:
            {
                std::int64_t valueFromStream{};
                str.readInt64(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_FLOAT:
            {
                float valueFromStream{};
                str.readFloat(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_DOUBLE:
            {
                double valueFromStream{};
                str.readDouble(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_STRING:
            {
                std::string valueFromStream{};
                str.readString(&valueFromStream, false);
                *this = valueFromStream;
                break;
            }
            case TYPE_BOOL:
            {
                bool valueFromStream{};
                str.readBool(&valueFromStream);
                *this = valueFromStream;
                break;
            }
            case TYPE_VEC2:
            {
                Vec2 v;
                v.read(str);
                *this = v;
                break;
            }
            case TYPE_OPAQUE:
                assert(false);
            case TYPE_VECTOR:
            {
                std::uint32_t size{};
                str.readUInt32(&size);
                (*this) = std::vector<Value>();
                for (std::size_t i=0; i<size; ++i)
                {
                    Value v;
                    v.readFromStream(str);
                    push_back(v);
                }
                break;
            }
            case TYPE_UNKNOWN:
                assert(false);
                break;
        }
        str.readFooter();
        
        return str;
    }
}
