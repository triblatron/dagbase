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

    class DAGBASE_API StreamFormat
    {
    public:
        virtual ~StreamFormat() = default;

        virtual void writeUInt32(BackingStore& store, std::string_view fieldName, std::uint32_t value) = 0;

        virtual void readUInt32(BackingStore& store, std::string *fieldName, std::uint32_t* value) = 0;
    };
}
#endif //STREAMFORMAT_H
