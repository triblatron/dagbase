//
// Created by tony on 07/03/24.
//
#include "config/config.h"

#include "io/OutputStream.h"

namespace dagbase
{
    OutputStream & OutputStream::writeVariableLengthInteger(const std::pmr::vector<std::uint8_t> &value)
    {
        for (auto b : value)
        {
            writeUInt8(b);
        }

        return *this;
    }
}
