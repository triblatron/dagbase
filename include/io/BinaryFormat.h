//
// Created by Tony Horrobin on 02/03/2025.
//
#pragma once

#ifndef BINARYFORMAT_H
#define BINARYFORMAT_H

#include "config/DagBaseExport.h"
#include "io/StreamFormat.h"

namespace dagbase
{
    class DAGBASE_API BinaryFormat : public StreamFormat
    {
    public:
        BinaryFormat(BackingStore* store);

        void setMode(Mode mode) override
        {
            _mode = mode;
        }

        Mode mode() const override
        {
            return _mode;
        }

        void flush() override;

        void writeUInt32(std::uint32_t value) override;
        void readUInt32(std::uint32_t *value) override;

        void writeInt64(std::int64_t value) override;
        void readInt64(std::int64_t* value) override;

        void writeDouble(double value) override;
        void readDouble(double* value) override;

        void writeString(std::string_view value, bool quoted) override;
        void readString(std::string* value, bool quoted) override;

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
        Mode _mode{MODE_UNKNOWN};
        BackingStore* _store{nullptr};
    };
}
#endif //BINARYFORMAT_H
