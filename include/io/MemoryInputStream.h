//
// Created by tony on 09/03/24.
//

#pragma once

#include "io/InputStream.h"

namespace dagbase
{
    class ByteBuffer;

    class DAGBASE_API MemoryInputStream : public InputStream
    {
    public:
        //! Construct from a buffer than has typically been filled by a MemoryOutputStream.
        explicit MemoryInputStream(ByteBuffer* buf);

        InputStream& readBuf(value_type* buf, std::size_t len) override;
    private:
        ByteBuffer* _buf;
    };
}
