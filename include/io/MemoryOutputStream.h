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

        OutputStream& writeUInt32(std::uint32_t value) override
        {
            writeBuf(reinterpret_cast<const value_type*>(&value), sizeof(uint32_t));
            return *this;
        }

        OutputStream& writeDouble(double value) override
        {
            writeBuf(reinterpret_cast<const value_type*>(&value), sizeof(double));
            return *this;
        }
    private:
        ByteBuffer* _buf{nullptr};
    };
}
