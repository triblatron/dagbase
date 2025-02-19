//
// Created by tony on 09/03/24.
//
#include "config/config.h"

#include "io/MemoryInputStream.h"
#include "core/ByteBuffer.h"

namespace dagbase
{
    MemoryInputStream::MemoryInputStream(ByteBuffer *buf)
    :
    _buf(buf)
    {
        // Do nothing.
    }

    InputStream& MemoryInputStream::readBuf(value_type* buf, std::size_t len)
    {
        if (_buf!=nullptr)
        {
            _buf->get(buf, len);
        }

        return *this;
    }
}