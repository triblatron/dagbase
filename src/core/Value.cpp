//
// Created by Tony Horrobin on 09/09/2026.
//

#include "config/config.h"

#include "core/Value.h"
#include "core/Editable.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"
#include "util/enums.h"

#include <cassert>

namespace dagbase
{
    Value::Value(const Value &other)
    {
        if (other.type() == TYPE_STRING && other.operator std::string*())
        {
            _value = new std::string(*other.operator std::string*());
        }
        else if (other.type() == TYPE_VECTOR && other.operator std::vector<Value>*())
        {
            _value = new std::vector<Value>(*other.operator std::vector<Value>*());
        }
        else
        {
            _value = other._value;
        }
    }

    Value::Value(Value &&value)
    {
        _value = value._value;
        value._value = (std::uint8_t)0;
    }

    Value::~Value()
    {
        if (type() == TYPE_STRING)
        {
            delete this->operator std::string*();
            *this = (std::string*)nullptr;
        }
        else if (type() == TYPE_VECTOR)
        {
            delete this->operator std::vector<Value>*();
            *this = (std::vector<Value>*)nullptr;
        }
    }

    Value & Value::operator=(const Value &value)
    {
        if (this != &value)
        {
            if (type() == TYPE_STRING)
            {
                delete this->operator std::string*();
            }
            else if (type() == TYPE_VECTOR)
            {
                delete this->operator std::vector<Value>*();
            }
            _value = value._value;
            if (type() == TYPE_STRING)
            {
                _value = new std::string(*this->operator std::string*());
            }
            else if (type() == TYPE_VECTOR)
            {
                _value = new std::vector<Value>(*this->operator std::vector<Value>*());
            }
        }

        return *this;
    }

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
            {
                Editable::editType(label, std::get<TYPE_STRING>(_value));
                break;
            }
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
                str.writeUInt8(this->operator std::uint8_t());
                break;
            case TYPE_INT8:
                str.writeInt8(this->operator std::int8_t());
                break;
            case TYPE_UINT16:
                str.writeUInt16(this->operator std::uint16_t());
                break;
            case TYPE_INT16:
                str.writeInt16(this->operator std::int16_t());
                break;
            case TYPE_UINT32:
                str.writeUInt32(this->operator std::uint32_t());
                break;
            case TYPE_INT32:
                str.writeInt32(this->operator std::int32_t());
                break;
            case TYPE_UINT64:
                str.writeUInt64(this->operator std::uint64_t());
                break;
            case TYPE_INT64:
                str.writeInt64(this->operator std::int64_t());
                break;
            case TYPE_FLOAT:
                str.writeFloat(this->operator float());
                break;
            case TYPE_DOUBLE:
                str.writeDouble(this->operator double());
                break;
            case TYPE_STRING:
                str.writeString(*static_cast<std::string*>(*this), false);
                break;
            case TYPE_BOOL:
                str.writeBool(this->operator bool());
                break;
            case TYPE_VEC2:
                this->operator Vec2().write(str);
                break;
            case TYPE_OPAQUE:
                assert(false);
            case TYPE_VECTOR:
            {
                const auto & vec = this->operator std::vector<Value>*();
                std::size_t size{0};
                if (vec)
                    size = vec->size();
                str.writeUInt32(size);
                if (vec)
                    for (const auto& v : *vec)
                    {
                        v.writeToStream(str);
                    }
                break;
            }
            case TYPE_UNKNOWN:
                assert(false);
                break;
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
                std::string* valueFromStream = new std::string();
                str.readString(valueFromStream, false);
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
                (*this) = new std::vector<Value>();
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

    Value::Type Value::parseType(const char* str)
	{
	    TEST_ENUM(TYPE_UINT8, str);
	    TEST_ENUM(TYPE_INT8, str);
	    TEST_ENUM(TYPE_UINT16, str);
	    TEST_ENUM(TYPE_INT16, str);
	    TEST_ENUM(TYPE_UINT32, str);
	    TEST_ENUM(TYPE_INT32, str);
	    TEST_ENUM(TYPE_UINT64, str);
	    TEST_ENUM(TYPE_INT64, str);
	    TEST_ENUM(TYPE_FLOAT, str);
	    TEST_ENUM(TYPE_DOUBLE, str);
	    TEST_ENUM(TYPE_STRING, str);
	    TEST_ENUM(TYPE_BOOL, str);
	    TEST_ENUM(TYPE_VEC2, str);
	    TEST_ENUM(TYPE_OPAQUE, str);
	    TEST_ENUM(TYPE_VECTOR, str);
	    TEST_ENUM(TYPE_UNKNOWN, str);

		return TYPE_UNKNOWN;
	}

    Value::Type Value::parseClass(const char *str)
    {
	    TEST_ALT_ENUM("TypedPort<uint8_t", TYPE_UINT8, str);
	    TEST_ALT_ENUM("TypedPort<int8_t>", TYPE_INT8, str);
	    TEST_ALT_ENUM("TypedPort<uint16_t>", TYPE_UINT16, str);
	    TEST_ALT_ENUM("TypedPort<int16_t>", TYPE_INT16, str);
	    TEST_ALT_ENUM("TypedPort<uint32_t", TYPE_UINT32, str);
	    TEST_ALT_ENUM("TypedPort<int32_t>", TYPE_INT32, str);
	    TEST_ALT_ENUM("TypedPort<uint64_t>", TYPE_UINT64, str);
	    TEST_ALT_ENUM("TypedPort<int64_t>", TYPE_INT64, str);
	    TEST_ALT_ENUM("TypedPort<float", TYPE_FLOAT, str);
	    TEST_ALT_ENUM("TypedPort<double>", TYPE_DOUBLE, str);
	    TEST_ALT_ENUM("TypedPort<string>", TYPE_STRING, str);
	    TEST_ALT_ENUM("TypedPort<bool>", TYPE_BOOL, str);
	    TEST_ALT_ENUM("TypedPort<Vec2>", TYPE_VEC2, str);
	    TEST_ALT_ENUM("TypedPort<void*>", TYPE_OPAQUE, str);
	    TEST_ALT_ENUM("TypedPort<vector>", TYPE_VECTOR, str);

        return TYPE_UNKNOWN;
    }

    const char *Value::typeString(Type type)
    {
        switch (type)
        {
            ENUM_NAME(TYPE_UINT8)
            ENUM_NAME(TYPE_INT8)
            ENUM_NAME(TYPE_UINT16)
            ENUM_NAME(TYPE_INT16)
            ENUM_NAME(TYPE_UINT32)
            ENUM_NAME(TYPE_INT32)
            ENUM_NAME(TYPE_UINT64)
            ENUM_NAME(TYPE_INT64)
            ENUM_NAME(TYPE_FLOAT)
            ENUM_NAME(TYPE_DOUBLE)
            ENUM_NAME(TYPE_STRING)
            ENUM_NAME(TYPE_BOOL)
            ENUM_NAME(TYPE_VEC2)
            ENUM_NAME(TYPE_OPAQUE)
            ENUM_NAME(TYPE_VECTOR)
            ENUM_NAME(TYPE_UNKNOWN)
        }

        return "<error>";
    }

    const char* Value::className(Type type)
    {
        switch (type)
        {
            ENUM_ALT_NAME(TYPE_UINT8, "TypedPort<uint8_t>")
            ENUM_ALT_NAME(TYPE_INT8, "TypedPort<int8_t>")
            ENUM_ALT_NAME(TYPE_UINT16, "TypedPort<uint16_t>")
            ENUM_ALT_NAME(TYPE_INT16, "TypedPort<int16_t>")
            ENUM_ALT_NAME(TYPE_UINT32, "TypedPort<uint32_t>")
            ENUM_ALT_NAME(TYPE_INT32, "TypedPort<int32_t>")
            ENUM_ALT_NAME(TYPE_UINT64, "TypedPort<uint64_t>")
            ENUM_ALT_NAME(TYPE_INT64, "TypedPort<int64_t>")
            ENUM_ALT_NAME(TYPE_FLOAT, "TypedPort<float>")
            ENUM_ALT_NAME(TYPE_DOUBLE, "TypedPort<double>")
            ENUM_ALT_NAME(TYPE_STRING, "TypedPort<string>")
            ENUM_ALT_NAME(TYPE_BOOL, "TypedPort<bool>")
            ENUM_ALT_NAME(TYPE_OPAQUE, "TypedPort<void*>")
            ENUM_ALT_NAME(TYPE_VEC2, "TypedPort<Vec2>")
            ENUM_ALT_NAME(TYPE_VECTOR, "TypedPort<vector>")
            ENUM_ALT_NAME(TYPE_UNKNOWN, "TypedPort<unknown>")
        }

        return "<error>";
    }

}
