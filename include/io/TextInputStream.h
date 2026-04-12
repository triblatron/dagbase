//
// Created by Tony Horrobin on 08/03/2025.
//

#pragma once

#include "config/DagBaseExport.h"
#include "io/InputStream.h"

#include <iosfwd>
#include <vector>

namespace dagbase
{
    class BackingStore;
}

namespace dagbase
{
    class DAGBASE_API TextInputStream : public InputStream
    {
    public:
        explicit TextInputStream(BackingStore* store);

        void setBackingStore(BackingStore* backingStore)
        {
            _store = backingStore;
        }

        InputStream& readBuf(value_type* buf, std::size_t len) override;

        InputStream& read(Lua& lua, Variant* value) override;

        InputStream& readHeader(std::string* className) override;

        InputStream& readField(std::string* fieldName) override;

        InputStream& readUInt8(std::uint8_t* value) override;

        InputStream& readInt8(std::int8_t* value) override;

        InputStream& readUInt16(std::uint16_t* value) override;

        InputStream& readInt16(std::int16_t* value) override;

        InputStream& readUInt32(std::uint32_t* value) override;

        InputStream& readInt32(std::int32_t* value) override;

        InputStream& readUInt64(std::uint64_t* value) override;

        InputStream& readInt64(int64_t* value) override;

        InputStream& readBool(bool* value) override;

        InputStream& readFloat(float* value) override;

        InputStream& readDouble(double* value) override;

        InputStream& readString(std::string* value, bool quoted) override;

        InputStream& readFooter() override;
    private:
        BackingStore* _store{ nullptr };
        std::istringstream* _istr{nullptr};
        //! The result of base64 encoding
        std::vector<std::uint8_t> _output;
    };
}
