//
// Created by Tony Horrobin on 04/03/2025.
//
#include "config/config.h"

#include "io/BinaryFormat.h"

#include "core/Class.h"
#include "io/BackingStore.h"

#include <cstring>

struct BufferUInt16
{
    std::uint8_t bytes[sizeof(std::uint16_t)];
};

struct BufferInt16
{
    std::uint8_t bytes[sizeof(std::int16_t)];
};

struct BufferUInt32
{
    std::uint8_t bytes[sizeof(std::uint32_t)];
};

struct BufferInt32
{
    std::uint8_t bytes[sizeof(std::int32_t)];
};

struct BufferFloat
{
    std::uint8_t bytes[sizeof(float)];
};

struct BufferDouble
{
    std::uint8_t bytes[sizeof(double)];
};

struct BufferUInt64
{
    std::uint8_t bytes[sizeof(std::uint64_t)];
};

struct BufferInt64
{
    std::uint8_t bytes[sizeof(std::int64_t)];
};

namespace dagbase
{
    BinaryFormat::BinaryFormat(BackingStore* store)
        :
    _store(store)
    {
        // Do nothing.
    }

    void BinaryFormat::flush()
    {
    }

    void BinaryFormat::writeUInt16(std::uint16_t value)
    {
        if (_store)
        {
            BufferUInt16 buffer{};
            memcpy(buffer.bytes, &value, sizeof(std::uint16_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readUInt16(std::uint16_t *value)
    {
        if (_store)
        {
            BufferUInt16 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::uint16_t));
        }
    }

    void BinaryFormat::writeInt16(std::int16_t value)
    {
        if (_store)
        {
            BufferInt16 buffer{};
            std::memcpy(buffer.bytes, &value, sizeof(std::int16_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readInt16(std::int16_t *value)
    {
        if (_store)
        {
            BufferInt16 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::int16_t));
        }
    }

    void BinaryFormat::writeUInt32(std::uint32_t value)
    {
        if (_store)
        {
            BufferUInt32 buffer{};
            std::memcpy(buffer.bytes, &value, sizeof(std::uint32_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readUInt32(std::uint32_t* value)
    {
        if (_store)
        {
            BufferUInt32 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::uint32_t));
        }
    }

    void BinaryFormat::writeInt32(std::int32_t value)
    {
        if (_store)
        {
            BufferInt32 buffer{};
            std::memcpy(buffer.bytes, &value, sizeof(std::int32_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readInt32(std::int32_t *value)
    {
        if (_store)
        {
            BufferInt32 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::int32_t));
        }
    }

    void BinaryFormat::writeUInt64(std::uint64_t value)
    {
        if (_store)
        {
            BufferUInt64 buffer{};
            std::memcpy(buffer.bytes, &value, sizeof(std::uint64_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readUInt64(std::uint64_t *value)
    {
        if (_store)
        {
            BufferUInt64 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::uint64_t));
        }
    }

    void BinaryFormat::writeInt64(std::int64_t value)
    {
        if (_store)
        {
            BufferInt64 buffer{};
            std::memcpy(buffer.bytes, &value, sizeof(std::int64_t));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readInt64(std::int64_t* value)
    {
        if (_store)
        {
            BufferInt64 buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(std::int64_t));
        }
    }

    void BinaryFormat::writeFloat(float value)
    {
        if (_store)
        {
            BufferFloat buffer{};
            std::memcpy(buffer.bytes, &value, sizeof(float));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readFloat(float *value)
    {
        if (_store)
        {
            BufferFloat buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(float));
        }
    }

    void BinaryFormat::writeDouble(double value)
    {
        if (_store)
        {
            BufferDouble buffer{};
            std::memcpy(buffer.bytes, &value, sizeof(double));
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readDouble(double* value)
    {
        if (_store)
        {
            BufferDouble buffer{};
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                std::memcpy(value, buffer.bytes, sizeof(double));
        }
    }

    void BinaryFormat::writeString(std::string_view value, bool quoted)
    {
        if (_store)
        {
            writeUInt32(value.length());
            _store->put(reinterpret_cast<const unsigned char*>(value.data()), value.length());
        }
    }

    void BinaryFormat::readString(std::string* value, bool quoted)
    {
        if (_store && value)
        {
            std::uint32_t length = 0;
            readUInt32(&length);
            value->resize(length);
            _store->get(reinterpret_cast<unsigned char*>(value->data()), length * sizeof(std::uint8_t));
        }
    }

    void BinaryFormat::writeBool(bool value)
    {
        if (_store)
        {
            _store->put(reinterpret_cast<const unsigned char*>(&value), sizeof(bool));
        }
    }

    void BinaryFormat::readBool(bool* value)
    {
        if (_store && value)
        {
            _store->get(reinterpret_cast<unsigned char*>(value), sizeof(bool));
        }
    }

    void BinaryFormat::writeField(const char* fieldName)
    {
        writeString(fieldName, false);
    }

    void BinaryFormat::readField(std::string* fieldName)
    {
        if (_store && fieldName)
        {
            readString(fieldName, false);
        }
    }

    void BinaryFormat::writeObject(Class* obj)
    {
        if (_store && obj)
        {
            obj->writeToStream(*this);
        }
    }

    void BinaryFormat::readObject(Class* obj)
    {
        if (_store && obj)
        {
            obj->readFromStream(*this);
        }
    }

    void BinaryFormat::writeHeader(const char* className)
    {
        if (_store && className)
        {
            writeString(className, false);
        }
    }

    void BinaryFormat::readHeader(std::string* className)
    {
        if (_store && className)
        {
            readString(className, false);
        }
    }

    void BinaryFormat::writeFooter()
    {
    }

    void BinaryFormat::readFooter()
    {
    }

    void BinaryFormat::debug()
    {
        // Do nothing.
    }

    void BinaryFormat::writeUInt8(std::uint8_t value)
    {
        if (_store)
            _store->put(&value, sizeof(std::uint8_t));
    }

    void BinaryFormat::readUInt8(std::uint8_t *value)
    {
        if (_store)
            _store->get(value, sizeof(std::uint8_t));
    }

    void BinaryFormat::writeInt8(std::int8_t value)
    {
        if (_store)
            _store->put(reinterpret_cast<std::uint8_t*>(&value), sizeof(std::int8_t));
    }

    void BinaryFormat::readInt8(std::int8_t *value)
    {
        if (_store)
            _store->get(reinterpret_cast<std::uint8_t*>(value), sizeof(std::int8_t));
    }

    void BinaryFormat::writeBinary(const std::uint8_t *buf, std::size_t len)
    {
        if (_store)
            _store->put(buf, len);
    }

    void BinaryFormat::readBinary(std::uint8_t *value, std::size_t len)
    {
        if (_store)
            _store->get(value, len);
    }
}
