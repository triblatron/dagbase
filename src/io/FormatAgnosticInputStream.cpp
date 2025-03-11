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
