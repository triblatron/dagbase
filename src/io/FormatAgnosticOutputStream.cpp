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

    OutputStream& FormatAgnosticOutputStream::write(ConfigurationElement::ValueType value)
    {
        if (_format && value.has_value())
        {
            switch (value->index())
            {
            case ConfigurationElement::TYPE_DOUBLE:
                writeDouble(std::get<double>(value.value()));
                break;
            case ConfigurationElement::TYPE_STRING:
                writeString(std::get<std::string>(value.value()),true);
                break;
            }
        }

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeUInt32(std::uint32_t value)
    {
        if (_format)
            _format->writeUInt32(value);

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeInt64(std::int64_t value)
    {
        if (_format)
        {
            _format->writeInt64(value);
        }

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeDouble(double value)
    {
        if (_format)
            _format->writeDouble(value);

        return *this;
    }

    OutputStream & FormatAgnosticOutputStream::writeString(const std::string &value, bool quoted)
    {
        if (_format)
            _format->writeString(value, quoted);

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
