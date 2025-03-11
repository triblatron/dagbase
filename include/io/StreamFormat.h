//
// Created by Tony Horrobin on 02/03/2025.
//
#pragma once

#ifndef STREAMFORMAT_H
#define STREAMFORMAT_H

#include "config/DagBaseExport.h"

#include <string_view>
#include <string>
#include <cstdint>

namespace dagbase
{
    class BackingStore;
    class Class;

    class DAGBASE_API StreamFormat
    {
    public:
        enum Mode
        {
            MODE_UNKNOWN,
            MODE_INPUT,
            MODE_OUTPUT
        };
    public:
        virtual ~StreamFormat() = default;

        virtual void setMode(Mode mode) = 0;

        virtual Mode mode() const = 0;

        virtual void flush() = 0;

        virtual void writeUInt32(std::uint32_t value) = 0;

        virtual void readUInt32(std::uint32_t* value) = 0;

        virtual void writeDouble(double value) = 0;

        virtual void readDouble(double* value) = 0;

        virtual void writeString(std::string_view value, bool quoted) = 0;

        virtual void readString(std::string* value, bool quoted) = 0;

        virtual void writeField(const char* fieldName) = 0;

        virtual void readField(std::string* fieldName) = 0;

        virtual void writeObject(Class* obj) = 0;

        virtual void readObject(Class* obj) = 0;

        virtual void writeHeader(const char* className) = 0;

        virtual void readHeader(std::string* className) = 0;

        virtual void writeFooter() = 0;

        virtual void readFooter() = 0;
    };
}
#endif //STREAMFORMAT_H
