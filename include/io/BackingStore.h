//
// Created by Tony Horrobin on 02/03/2025.
//

#pragma once

#ifndef BACKINGSTORE_H
#define BACKINGSTORE_H

#include "config/DagBaseExport.h"

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

namespace dagbase
{
    class DAGBASE_API BackingStore
    {
    public:
        enum Mode : std::uint32_t
        {
            MODE_INPUT_BIT = 1<<0,
            MODE_OUTPUT_BIT = 1<<1,
            MODE_TEXT_BIT = 1<<2,
            MODE_BINARY_BIT = 1<<3,
        };
    public:
        BackingStore(Mode mode)
            :
        _mode(mode)
        {
            // Do nothing.
        }

        virtual ~BackingStore() = default;

        void setMode(Mode mode)
        {
            _mode = mode;
        }

        Mode mode() const
        {
            return _mode;
        }

        virtual void put(const unsigned char* buffer, std::size_t bufferSize) = 0;

        virtual void get(unsigned char* buffer, std::size_t bufferSize) = 0;

        virtual void get(std::string& buffer, unsigned char delim) = 0;

        virtual const unsigned char* buffer() const = 0;
    private:
        Mode _mode{0x0};
    };
}
#endif //BACKINGSTORE_H
