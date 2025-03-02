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
        void writeUInt32(BackingStore& store, std::string_view fieldName, std::uint32_t value) override;
        void readUInt32(BackingStore& store, std::string *fieldName, std::uint32_t *value) override;
    };
}
#endif //BINARYFORMAT_H
