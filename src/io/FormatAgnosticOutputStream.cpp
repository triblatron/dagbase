//
// Created by Tony Horrobin on 08/03/2025.
//

#include "config/config.h"

#include "io/FormatAgnosticOutputStream.h"

#include "io/StreamFormat.h"
#include "io/BackingStore.h"

namespace dagbase
{
    FormatAgnosticOutputStream::FormatAgnosticOutputStream(StreamFormat* format, BackingStore* store)
        :
    _format(format),
    _backingStore(store)
    {
        if (_backingStore)
        {
            _backingStore->setMode(BackingStore::MODE_OUTPUT_BIT);
        }
        if (_format)
        {
            _format->setMode(StreamFormat::MODE_OUTPUT);
        }
    }

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
            switch (value.index())
            {
            case Variant::TYPE_DOUBLE:
                writeDouble(value.asDouble());
                break;
            case Variant::TYPE_STRING:
                writeString(value.asString(),true);
                break;
            case Variant::TYPE_BOOL:
                writeBool(value.asBool());
                break;
            case Variant::TYPE_INTEGER:
                writeInt64(value.asInteger());
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

    OutputStream& FormatAgnosticOutputStream::writeBool(bool value)
    {
        if (_format)
            _format->writeBool(value);

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
