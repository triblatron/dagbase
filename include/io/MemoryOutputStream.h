//
// Created by tony on 07/03/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include "io/OutputStream.h"

namespace dagbase
{
    class ByteBuffer;

    class DAGBASE_API MemoryOutputStream : public OutputStream
    {
    public:
        //! Construct from a byffer that is typically empty.
        explicit MemoryOutputStream(ByteBuffer* buf);

        OutputStream& writeBuf(const value_type* buf, std::size_t len) override;
    private:
        ByteBuffer* _buf{nullptr};
    };
}
