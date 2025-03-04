//
// Created by Tony Horrobin on 02/03/2025.
//

#include "config/config.h"

#include "io/TextFormat.h"
#include "io/BackingStore.h"
#include "util/PrettyPrinter.h"

#include <sstream>

namespace dagbase
{
    void TextFormat::writeUInt32(BackingStore& store, std::uint32_t value)
    {
        std::ostringstream str;

        str << value << '\n';
        std::string strValue = str.str();
        store.put(reinterpret_cast<const unsigned char*>(strValue.c_str()), strValue.size());
    }

    void TextFormat::readUInt32(BackingStore& store, std::uint32_t* value)
    {
        std::string input;
        std::uint32_t temp{0};

        store.get(input, '\n');

        std::istringstream str(input);
        str >> temp;

        if (value)
            *value = temp;
    }
}
