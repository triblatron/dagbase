//
// Created by Tony Horrobin on 02/03/2025.
//

#pragma once

#ifndef MEMORYBACKINGSTORE_H
#define MEMORYBACKINGSTORE_H

#include "config/DagBaseExport.h"

#include "io/BackingStore.h"
#include "core/ByteBuffer.h"

#include <string>

namespace dagbase
{
    class DAGBASE_API MemoryBackingStore : public BackingStore
    {
    public:
        MemoryBackingStore(Mode mode);

        void open(Mode mode)
        {
            setMode(mode);
            if ((mode & MODE_OUTPUT_BIT) != 0)
            {
                _buffer.clear();
            }
            else if ((mode & MODE_INPUT_BIT) != 0)
            {
                _buffer.flip();
            }
        }

        void get(std::string& buffer, unsigned char delim) override;
        void get(unsigned char* buffer, std::size_t bufferSize) override;
        void put(const unsigned char* buffer, std::size_t bufferSize) override;
        const unsigned char* buffer() const override
        {
            return  _buffer.buffer();
        }

    private:
        ByteBuffer _buffer;
    };
}

#endif //MEMORYBACKINGSTORE_H
