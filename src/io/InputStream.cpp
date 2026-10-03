//
// Created by tony on 09/03/24.
//
#include "config/config.h"

#include "io/InputStream.h"

namespace dagbase
{
    InputStream & InputStream::readVariableLengthInteger(std::size_t moreBit, std::uint64_t* value)
    {
        std::uint8_t moreBitMask = 1<<moreBit;
        std::uint8_t byteFromStream{0};
        readUInt8(&byteFromStream);
        auto decoded = static_cast<std::uint64_t>(byteFromStream & ~moreBitMask);
        std::uint8_t shift = moreBit;
        while ((byteFromStream & moreBitMask)!=0)
        {
            readUInt8(&byteFromStream);
            decoded |= (byteFromStream & 127u) << shift;
            shift += 7;
            moreBitMask = 1<<7;
        }
        if (value)
        {
            *value = decoded;
        }

        return *this;
    }
}
