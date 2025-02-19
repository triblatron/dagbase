//
// Created by tony on 07/03/24.
//
#include "config/config.h"

#include "io/MemoryOutputStream.h"
#include "core/ByteBuffer.h"

namespace dagbase
{
    MemoryOutputStream::MemoryOutputStream(ByteBuffer *buf)
            :
            _buf(buf)
    {
        // Do nothing.
    }

    OutputStream& MemoryOutputStream::writeBuf(const OutputStream::value_type *buf, std::size_t len)
    {
        if (_buf!=nullptr)
        {
            _buf->put(buf, len);
        }

        return *this;
    }
}
