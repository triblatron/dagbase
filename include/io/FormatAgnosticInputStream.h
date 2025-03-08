//
// Created by Tony Horrobin on 08/03/2025.
//

#pragma once

#include "InputStream.h"
#include "config/DagBaseExport.h"
#include "io/OutputStream.h"

namespace dagbase
{
    class BackingStore;
}

namespace dagbase
{
    class StreamFormat;
}

namespace dagbase
{
    class DAGBASE_API FormatAgnosticInputStream : public InputStream
    {
    public:
        void setFormat(StreamFormat* format)
        {
            _format = format;
        }

        void setBackingStore(BackingStore* backingStore)
        {
            _backingStore = backingStore;
        }

        InputStream& readBuf(value_type* buf, std::size_t len) override;

        InputStream& readHeader(std::string* className) override;

        InputStream& readField(std::string* fieldName) override;

        InputStream& readUInt32(std::uint32_t* value) override;

        InputStream& readFooter() override;
    private:
        StreamFormat* _format{ nullptr };
        BackingStore* _backingStore{ nullptr };
    };
}
