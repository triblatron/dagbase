//
// Created by Tony Horrobin on 12/04/2026.
//

#include "config/config.h"

#include "io/BinaryInputStream.h"
#include "io/BackingStore.h"
#include "util/TypedBuffer.h"

namespace dagbase
{
    BinaryInputStream::BinaryInputStream(BackingStore *store)
        :
    _store(store)
    {
        // Do nothing.
    }

    InputStream & BinaryInputStream::readBuf(value_type *buf, std::size_t len)
    {
        if (_store)
            _store->get(buf, len);

        return *this;
    }

    InputStream & BinaryInputStream::read(Lua &lua, Variant *value)
    {
        if (value)
        {
            value->read(*this, lua);
        }

        return *this;
    }

    InputStream & BinaryInputStream::readHeader(std::string *className)
    {
        readString(className, false);

        return *this;
    }

    InputStream & BinaryInputStream::readField(std::string *fieldName)
    {
        if (_store && fieldName)
        {
            readString(fieldName, false);
        }

        return *this;
    }

    InputStream & BinaryInputStream::readUInt8(std::uint8_t *value)
    {
        if (_store)
            _store->get(value, sizeof(std::uint8_t));

        return *this;
    }

    InputStream & BinaryInputStream::readInt8(std::int8_t *value)
    {
        if (_store)
            _store->get(reinterpret_cast<std::uint8_t*>(value), sizeof(std::int8_t));

        return *this;
    }

    InputStream & BinaryInputStream::readUInt16(std::uint16_t *value)
    {
        if (_store)
        {
            BufferUInt16 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::uint16_t));
        }
        return *this;
    }

    InputStream & BinaryInputStream::readInt16(std::int16_t *value)
    {
        if (_store)
        {
            BufferInt16 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::int16_t));
        }

        return *this;
    }

    InputStream & BinaryInputStream::readUInt32(std::uint32_t *value)
    {
        if (_store)
        {
            BufferUInt32 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::uint32_t));
        }

        return *this;
    }

    InputStream & BinaryInputStream::readInt32(std::int32_t *value)
    {
        if (_store)
        {
            BufferInt32 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::int32_t));
        }

        return *this;
    }

    InputStream & BinaryInputStream::readUInt64(std::uint64_t *value)
    {
        if (_store)
        {
            BufferUInt64 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::uint64_t));
        }

        return *this;
    }

    InputStream & BinaryInputStream::readInt64(int64_t *value)
    {
        if (_store)
        {
            BufferInt64 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::int64_t));
        }

        return *this;
    }

    InputStream & BinaryInputStream::readBool(bool *value)
    {
        if (_store && value)
        {
            _store->get(reinterpret_cast<unsigned char*>(value), sizeof(bool));
        }

        return *this;
    }

    InputStream & BinaryInputStream::readFloat(float *value)
    {
        if (_store)
        {
            BufferFloat buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(float));
        }

        return *this;
    }

    InputStream & BinaryInputStream::readDouble(double *value)
    {
        if (_store)
        {
            BufferDouble buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(double));
        }

        return *this;
    }

    InputStream & BinaryInputStream::readString(std::string *value, bool quoted)
    {
        if (_store && value)
        {
            std::uint32_t length = 0;
            readUInt32(&length);
            value->resize(length);
            _store->get(reinterpret_cast<unsigned char*>(value->data()), length * sizeof(std::uint8_t));
        }

        return *this;
    }

    InputStream & BinaryInputStream::readFooter()
    {
        return *this;
    }
}
