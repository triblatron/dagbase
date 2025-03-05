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

        void writeUInt32(BackingStore& store, std::uint32_t value) override;
        void readUInt32(BackingStore& store, std::uint32_t *value) override;

        void writeString(BackingStore& store, std::string_view value) override;
        void readString(BackingStore& store, std::string* value) override;

        void writeField(BackingStore& store, const char* fieldName) override;
        void readField(BackingStore& store, std::string* fieldName) override;

        void writeObject(BackingStore& store, Class* obj) override;
        void readObject(BackingStore& store, Class* obj) override;

        void writeHeader(BackingStore& store, const char* className) override;
        void readHeader(BackingStore& store, std::string* className) override;

        void writeFooter(BackingStore& store) override;
        void readFooter(BackingStore& store) override;
    private:
        Mode _mode{MODE_UNKNOWN};
        BackingStore* _store{nullptr};
    };
}
#endif //BINARYFORMAT_H
