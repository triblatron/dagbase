//
// Created by Tony Horrobin on 08/03/2025.
//

#include "config/config.h"

#include "io/FormatAgnosticOutputStream.h"

#include "io/StreamFormat.h"
#include "io/BackingStore.h"

namespace dagbase
{
    OutputStream& FormatAgnosticOutputStream::writeBuf(const value_type* buf, std::size_t len)
    {
        if (_backingStore)
        {
            _backingStore->put(buf, len);
        }
        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeUInt32(std::uint32_t value)
    {
        if (_format)
            _format->writeUInt32(value);

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeHeader(std::string_view className)
    {
        if (_format)
            _format->writeHeader(className.data());

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeFooter()
    {
        if (_format)
            _format->writeFooter();

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeField(std::string_view name)
    {
        if (_format)
            _format->writeField(name.data());

        return *this;
    }
}
