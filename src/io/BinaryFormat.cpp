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
}
