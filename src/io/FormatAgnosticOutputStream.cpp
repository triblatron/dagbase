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
    _store(store)
    {
        if (_store)
        {
            _store->setMode(BackingStore::MODE_OUTPUT_BIT);
        }
        if (_format)
        {
            _format->setMode(StreamFormat::MODE_OUTPUT);
        }
    }

    OutputStream& FormatAgnosticOutputStream::writeBuf(const value_type* buf, std::size_t len)
    {
        if (_format)
        {
            if (_inSubBuffer)
            {
                _subBuffer.put(buf, len);
            }
            else
            {
                _format->writeBinary(buf, len);
            }
        }

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::write(Variant value)
    {
        if (_format)
        {
            value.write(*this);
        }

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeUInt8(std::uint8_t value)
    {
        if (_format)
            _format->writeUInt8(value);

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeInt8(std::int8_t value)
    {
        if (_format)
            _format->writeInt8(value);

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeUInt16(std::uint16_t value)
    {
        if (_format)
            _format->writeUInt16(value);

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeInt16(std::int16_t value)
    {
        if (_format)
            _format->writeInt16(value);

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeUInt32(std::uint32_t value)
    {
        if (_format)
            _format->writeUInt32(value);

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeInt32(std::int32_t value)
    {
        if (_format)
            _format->writeInt32(value);

        return *this;
    }

    OutputStream& FormatAgnosticOutputStream::writeUInt64(std::uint64_t value)
    {
        if (_format)
            _format->writeUInt64(value);

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

    OutputStream & FormatAgnosticOutputStream::writeFloat(float value)
    {
        if (_format)
            _format->writeFloat(value);

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

    OutputStream & FormatAgnosticOutputStream::flush()
    {
        if (_format)
            _format->flush();

        return *this;
    }

    OutputStream &FormatAgnosticOutputStream::beginSubBuffer()
    {
        return *this;
    }

    OutputStream &FormatAgnosticOutputStream::endSubBuffer()
    {
        return *this;
    }
}
