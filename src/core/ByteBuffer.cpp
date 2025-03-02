//
// Created by tony on 05/03/24.
//
#include "config/config.h"

#include "core/ByteBuffer.h"

namespace dagbase
{
    ByteBuffer &ByteBuffer::clear()
    {
        _position = 0;
        _limit = _buf.size();

        return *this;
    }

    ByteBuffer &ByteBuffer::flip()
    {
        _limit = _position;
        _position = 0;

        return *this;
    }
}
