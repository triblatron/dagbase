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

        void writeUInt32(std::uint32_t value) override;
        void readUInt32(std::uint32_t* value) override;

        void writeString(std::string_view value) override;
        void readString(std::string* value) override;

        void writeField(const char* fieldName) override;
        void readField(std::string* fieldName) override;

        void writeObject(Class* obj) override;
        void readObject(Class* obj) override;

        void writeHeader(const char* className) override;
        void readHeader(std::string* className) override;

        void writeFooter() override;
        void readFooter() override;
    private:
        Mode _mode{MODE_UNKNOWN};
        BackingStore* _store{nullptr};
        std::ostringstream* _ostr{nullptr};
        std::istringstream* _istr{nullptr};
        DebugPrinter* _printer{nullptr};
    };
}

#endif //TEXTFORMAT_H
