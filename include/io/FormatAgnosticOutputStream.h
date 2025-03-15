//
// Created by Tony Horrobin on 08/03/2025.
//

#pragma once

#include "config/DagBaseExport.h"
#include "OutputStream.h"

namespace dagbase
{
    class BackingStore;
    class StreamFormat;

    class DAGBASE_API FormatAgnosticOutputStream : public dagbase::OutputStream
    {
    public:
        FormatAgnosticOutputStream() = default;

        void setFormat(StreamFormat* format)
        {
            _format = format;
        }

        void setBackingStore(BackingStore* store)
        {
            _backingStore = store;
        }

        OutputStream& writeBuf(const value_type* buf, std::size_t len) override;

        OutputStream& write(ConfigurationElement::ValueType value) override;

        OutputStream& writeUInt32(std::uint32_t value) override;

        OutputStream& writeInt64(std::int64_t value) override;

        OutputStream& writeDouble(double value) override;

        OutputStream& writeString(const std::string& value, bool quoted) override;

        OutputStream& writeHeader(std::string_view className) override;

        OutputStream& writeFooter() override;

        OutputStream& writeField(std::string_view name) override;
    private:
        StreamFormat* _format{ nullptr };
        BackingStore* _backingStore{ nullptr };
    };
}
