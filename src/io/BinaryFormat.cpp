//
// Created by Tony Horrobin on 04/03/2025.
//
#include "config/config.h"

#include "io/BinaryFormat.h"

#include "core/Class.h"
#include "io/BackingStore.h"

union BufferUInt32
{
    std::uint32_t value;
    std::uint8_t bytes[sizeof(value)];
};

union BufferDouble
{
    double value;
    std::uint8_t bytes[sizeof(value)];
};

union BufferInt64
{
    std::int64_t value;
    std::uint8_t bytes[sizeof(value)];
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

    void BinaryFormat::writeUInt32(std::uint32_t value)
    {
        if (_store)
        {
            BufferUInt32 buffer;
            buffer.value = value;
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readUInt32(std::uint32_t* value)
    {
        if (_store)
        {
            BufferUInt32 buffer;
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                *value = buffer.value;
        }
    }

    void BinaryFormat::writeInt64(std::int64_t value)
    {
        if (_store)
        {
            BufferInt64 buffer;
            buffer.value = value;
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readInt64(std::int64_t* value)
    {
        if (_store)
        {
            BufferInt64 buffer;
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                *value = buffer.value;
        }
    }

    void BinaryFormat::writeDouble(double value)
    {
        if (_store)
        {
            BufferDouble buffer;
            buffer.value = value;
            _store->put(buffer.bytes, sizeof(buffer.bytes));
        }
    }

    void BinaryFormat::readDouble(double* value)
    {
        if (_store)
        {
            BufferDouble buffer;
            _store->get(buffer.bytes, sizeof(buffer.bytes));
            if (value)
                *value = buffer.value;
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
}
