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

    void BinaryFormat::writeString(std::string_view value)
    {
        if (_store)
        {
            writeUInt32(value.length());
            _store->put(reinterpret_cast<const unsigned char*>(value.data()), value.length());
        }
    }

    void BinaryFormat::readString(std::string* value)
    {
        if (_store && value)
        {
            std::uint32_t length = 0;
            readUInt32(&length);
            value->resize(length);
            _store->get(reinterpret_cast<unsigned char*>(value->data()), length * sizeof(std::uint8_t));
        }
    }

    void BinaryFormat::writeField(const char* fieldName)
    {
        writeString(fieldName);
    }

    void BinaryFormat::readField(std::string* fieldName)
    {
        if (_store && fieldName)
        {
            readString(fieldName);
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
            writeString(className);
        }
    }

    void BinaryFormat::readHeader(std::string* className)
    {
        if (_store && className)
        {
            readString(className);
        }
    }

    void BinaryFormat::writeFooter()
    {
    }

    void BinaryFormat::readFooter()
    {
    }
}
