//
// Created by Tony Horrobin on 02/03/2025.
//

#pragma once

#ifndef TEXTFORMAT_H
#define TEXTFORMAT_H

#include "config/DagBaseExport.h"

#include "io/StreamFormat.h"

namespace dagbase
{
    class DAGBASE_API TextFormat : public StreamFormat
    {
    public:
        void writeUInt32(BackingStore& store, std::string_view fieldName, std::uint32_t value) override;
        void readUInt32(BackingStore& store, std::string* fieldName, std::uint32_t* value) override;
    };
}

#endif //TEXTFORMAT_H
