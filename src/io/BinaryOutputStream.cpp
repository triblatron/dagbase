//
// Created by Tony Horrobin on 12/04/2026.
//

#include "io/BinaryOutputStream.h"
#include "io/BackingStore.h"
#include "util/TypedBuffer.h"

namespace dagbase
{
    BinaryOutputStream::BinaryOutputStream(BackingStore *store)
        :
    _store(store)
    {
        // Do nothing.
    }

    OutputStream & BinaryOutputStream::beginSubBuffer()
    {
        _inSubBuffer = true;
        return *this;
    }

    OutputStream & BinaryOutputStream::endSubBuffer()
    {
        _inSubBuffer = false;
        return *this;
    }

    OutputStream & BinaryOutputStream::writeBuf(const value_type *buf, std::size_t len)
    {
        if (_store)
            _store->put(buf, len);

        return *this;
    }

    OutputStream & BinaryOutputStream::write(Variant value)
    {
        value.write(*this);

        return *this;
    }

    OutputStream & BinaryOutputStream::writeUInt8(std::uint8_t value)
    {
        if (_store)
        {
            _store->put(&value, sizeof(std::uint8_t));
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeInt8(std::int8_t value)
    {
        if (_store)
            _store->put(reinterpret_cast<std::uint8_t*>(&value), sizeof(std::int8_t));

        return *this;
    }

    OutputStream & BinaryOutputStream::writeUInt16(std::uint16_t value)
    {
        if (_store)
        {
            BufferUInt16 buffer{};
            memcpy(buffer.bytes, &value, sizeof(std::uint16_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeInt16(std::int16_t value)
    {
        if (_store)
        {
            BufferInt16 buffer{};
            memcpy(buffer.bytes, &value, sizeof(std::int16_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeUInt32(std::uint32_t value)
    {
        if (_store)
        {
            BufferUInt32 buffer{};
            memcpy(buffer.bytes, &value, sizeof(std::uint32_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeInt32(std::int32_t value)
    {
        if (_store)
        {
            BufferInt32 buffer{};
            memcpy(buffer.bytes, &value, sizeof(std::int32_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeUInt64(std::uint64_t value)
    {
        if (_store)
        {
            BufferUInt64 buffer{};
            memcpy(buffer.bytes, &value, sizeof(std::uint64_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeInt64(std::int64_t value)
    {
        if (_store)
        {
            BufferInt64 buffer{};
            memcpy(buffer.bytes, &value, sizeof(std::int64_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeBool(bool value)
    {
        if (_store)
        {
            _store->put(reinterpret_cast<const unsigned char*>(&value), sizeof(bool));
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeFloat(float value)
    {
        if (_store)
        {
            BufferFloat buffer{};
            memcpy(buffer.bytes, &value, sizeof(float));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeDouble(double value)
    {
        if (_store)
        {
            BufferDouble buffer{};
            memcpy(buffer.bytes, &value, sizeof(double));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeString(const std::string &value, bool quoted)
    {
        if (_store)
        {
            writeUInt32(value.length());
            _store->put(reinterpret_cast<const unsigned char*>(value.data()), value.length());
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeHeader(std::string_view className)
    {
        if (_store)
        {
            writeString(std::string(className), false);
        }

        return *this;
    }

    OutputStream & BinaryOutputStream::writeFooter()
    {
        return *this;
    }

    OutputStream & BinaryOutputStream::writeField(std::string_view name)
    {
        writeString(std::string(name), false);

        return *this;
    }

    OutputStream & BinaryOutputStream::flush()
    {
        // Do nothing.

        return *this;
    }
}
