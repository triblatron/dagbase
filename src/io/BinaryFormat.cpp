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

    void BinaryFormat::writeUInt32(BackingStore& store, std::uint32_t value)
    {
        BufferUInt32 buffer;
        buffer.value = value;
        store.put(buffer.bytes, sizeof(buffer.bytes));
    }

    void BinaryFormat::readUInt32(BackingStore& store, std::uint32_t* value)
    {
        BufferUInt32 buffer;
        store.get(buffer.bytes, sizeof(buffer.bytes));
        if (value)
            *value = buffer.value;
    }

    void BinaryFormat::writeString(BackingStore& store, std::string_view value)
    {
        if (_store)
        {
            writeUInt32(store, value.length());
            _store->put(reinterpret_cast<const unsigned char*>(value.data()), value.length());
        }
    }

    void BinaryFormat::readString(BackingStore& store, std::string* value)
    {
        if (_store && value)
        {
            std::uint32_t length = 0;
            readUInt32(store, &length);
            value->resize(length);
            _store->get(reinterpret_cast<unsigned char*>(value->data()), length * sizeof(std::uint8_t));
        }
    }

    void BinaryFormat::writeField(BackingStore& store, const char* fieldName)
    {
        writeString(store, fieldName);
    }

    void BinaryFormat::readField(BackingStore& store, std::string* fieldName)
    {
        if (_store && fieldName)
        {
            readString(store, fieldName);
        }
    }

    void BinaryFormat::writeObject(BackingStore& store, Class* obj)
    {
        if (_store && obj)
        {
            obj->writeToStream(store, *this);
        }
    }

    void BinaryFormat::readObject(BackingStore& store, Class* obj)
    {
        if (_store && obj)
        {
            obj->readFromStream(store, *this);
        }
    }

    void BinaryFormat::writeHeader(BackingStore& store, const char* className)
    {
        if (_store && className)
        {
            writeString(store, className);
        }
    }

    void BinaryFormat::readHeader(BackingStore& store, std::string* className)
    {
        if (_store && className)
        {
            readString(store, className);
        }
    }

    void BinaryFormat::writeFooter(BackingStore& store)
    {
    }

    void BinaryFormat::readFooter(BackingStore& store)
    {
    }
}
