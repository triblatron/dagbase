//
// Created by Tony Horrobin on 02/03/2025.
//

#include "config/config.h"

#include "io/MemoryBackingStore.h"

namespace dagbase
{
    MemoryBackingStore::MemoryBackingStore(Mode mode)
        :
    BackingStore(mode)
    {
        // Do nothing.
    }

    void MemoryBackingStore::get(std::string& buffer, unsigned char delim)
    {
        unsigned char c = '\0';

        while (c != delim)
        {
            _buffer.get(&c, 1);
            if (c!=delim)
                buffer += c;
        }
    }

    void MemoryBackingStore::get(unsigned char* buffer, std::size_t bufferSize)
    {
        _buffer.get(buffer, bufferSize);
    }

    void MemoryBackingStore::put(const unsigned char* buffer, std::size_t bufferSize)
    {
        _buffer.put(buffer, bufferSize);
    }
}
