//
// Created by Tony Horrobin on 02/03/2025.
//

#pragma once

#ifndef TEXTFORMAT_H
#define TEXTFORMAT_H

#include "config/DagBaseExport.h"

#include "io/StreamFormat.h"

#include <iosfwd>

namespace dagbase
{
    class DebugPrinter;

    class DAGBASE_API TextFormat : public StreamFormat
    {
    public:
        TextFormat(BackingStore* store);

        void setMode(Mode mode) override;

        Mode mode() const override
        {
            return _mode;
        }

        void flush() override;

        void writeBinary(const std::uint8_t* buf, std::size_t len) override;
        void readBinary(std::uint8_t* value, std::size_t len) override;

        void writeUInt8(std::uint8_t value) override;
        void readUInt8(std::uint8_t* value) override;

        void writeUInt32(std::uint32_t value) override;
        void readUInt32(std::uint32_t* value) override;

        void writeInt64(std::int64_t value) override;
        void readInt64(std::int64_t* value) override;

        void writeDouble(double value) override;
        void readDouble(double* value) override;

        void writeString(std::string_view value, bool quoted) override;
        void readString(std::string* value, bool quoted) override;

        void writeBool(bool value) override;
        void readBool(bool* value) override;

        void writeField(const char* fieldName) override;
        void readField(std::string* fieldName) override;

        void writeObject(Class* obj) override;
        void readObject(Class* obj) override;

        void writeHeader(const char* className) override;
        void readHeader(std::string* className) override;

        void writeFooter() override;
        void readFooter() override;

        void debug() override;
    private:
        BackingStore* _store{nullptr};
        std::ostringstream* _ostr{nullptr};
        std::istringstream* _istr{nullptr};
        DebugPrinter* _printer{nullptr};
        Mode _mode{MODE_UNKNOWN};
    };
}

#endif //TEXTFORMAT_H
