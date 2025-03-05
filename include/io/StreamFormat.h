//
// Created by Tony Horrobin on 02/03/2025.
//
#pragma once

#ifndef STREAMFORMAT_H
#define STREAMFORMAT_H

#include <__fwd/string_view.h>

#include "config/DagBaseExport.h"

#include <string_view>
#include <string>

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

        virtual void writeUInt32(BackingStore& store, std::uint32_t value) = 0;

        virtual void readUInt32(BackingStore& store, std::uint32_t* value) = 0;

        virtual void writeString(BackingStore& store, std::string_view value) = 0;

        virtual void readString(BackingStore& store, std::string* value) = 0;

        virtual void writeField(BackingStore& store, const char* fieldName) = 0;

        virtual void readField(BackingStore& store, std::string* fieldName) = 0;

        virtual void writeObject(BackingStore& store, Class* obj) = 0;

        virtual void readObject(BackingStore& store, Class* obj) = 0;

        virtual void writeHeader(BackingStore& store, const char* className) = 0;

        virtual void readHeader(BackingStore& store, std::string* className) = 0;

        virtual void writeFooter(BackingStore& store) = 0;

        virtual void readFooter(BackingStore& store) = 0;
    };
}
#endif //STREAMFORMAT_H
