//
// Created by Tony Horrobin on 08/03/2025.
//

#include "config/config.h"

#include "io/FormatAgnosticInputStream.h"
#include "io/StreamFormat.h"
#include "io/BackingStore.h"

namespace dagbase
{
    InputStream& FormatAgnosticInputStream::readBuf(value_type* buf, std::size_t len)
    {
        if (_backingStore)
        {
            _backingStore->get(buf, len);
        }
        return *this;
    }

    InputStream& FormatAgnosticInputStream::read(ConfigurationElement::ValueType* value)
    {
        if (_format && value && value->has_value())
        {
            switch (value->value().index())
            {
            case ConfigurationElement::TYPE_DOUBLE:
                {
                    double doubleValue{0.0};
                    readDouble(&doubleValue);
                    *value = doubleValue;
                }
                break;
            case ConfigurationElement::TYPE_STRING:
                {
                    std::string stringValue;
                    readString(&stringValue, true);
                    *value = stringValue;
                }
                break;
            }
        }

        return *this;
    }

    InputStream& FormatAgnosticInputStream::readHeader(std::string* className)
    {
        if (_format)
        {
            _format->readHeader(className);
        }
        return *this;
    }

    InputStream& FormatAgnosticInputStream::readField(std::string* fieldName)
    {
        if (_format)
        {
            _format->readField(fieldName);
        }

        return *this;
    }

    InputStream& FormatAgnosticInputStream::readUInt32(std::uint32_t* value)
    {
        if (_format)
        {
            _format->readUInt32(value);
        }

        return *this;
    }

    InputStream& FormatAgnosticInputStream::readInt64(std::int64_t* value)
    {
        if (_format)
        {
            _format->readInt64(value);
        }

        return *this;
    }

    InputStream& FormatAgnosticInputStream::readDouble(double* value)
    {
        if (_format)
        {
            _format->readDouble(value);
        }

        return *this;
    }

    InputStream & FormatAgnosticInputStream::readString(std::string *value, bool quoted)
    {
        if (_format)
        {
            _format->readString(value, quoted);
        }

        return *this;
    }

    InputStream& FormatAgnosticInputStream::readFooter()
    {
        if (_format)
        {
            _format->readFooter();
        }

        return *this;
    }
}
