//
// Created by Tony Horrobin on 04/03/2025.
//
#include "config/config.h"

#include "io/BinaryFormat.h"
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
    }

    void BinaryFormat::readString(BackingStore& store, std::string* value)
    {
    }

    void BinaryFormat::writeField(BackingStore& store, const char* fieldName)
    {
    }

    void BinaryFormat::readField(BackingStore& store, std::string* fieldName)
    {
    }

    void BinaryFormat::writeObject(BackingStore& store, Class* obj)
    {
    }

    void BinaryFormat::readObject(BackingStore& store, Class* obj)
    {
    }

    void BinaryFormat::writeHeader(BackingStore& store, const char* className)
    {
    }

    void BinaryFormat::readHeader(BackingStore& store, std::string* className)
    {
    }

    void BinaryFormat::writeFooter(BackingStore& store)
    {
    }

    void BinaryFormat::readFooter(BackingStore& store)
    {
    }
}
