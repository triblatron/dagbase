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
        void writeUInt32(BackingStore& store, std::uint32_t value) override;
        void readUInt32(BackingStore& store, std::uint32_t* value) override;

        void writeObject(BackingStore& store, Class* obj) override;
        void readObject(BackingStore& store, Class* obj) override;
    };
}

#endif //TEXTFORMAT_H
