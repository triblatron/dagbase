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
    void TextFormat::writeUInt32(BackingStore& store, std::string_view fieldName, std::uint32_t value)
    {
        std::ostringstream str;

        str << fieldName << ':' << value << '\n';
        std::string strValue = str.str();
        store.put(reinterpret_cast<const unsigned char*>(strValue.c_str()), strValue.size());
    }

    void TextFormat::readUInt32(BackingStore& store, std::string* fieldName, std::uint32_t* value)
    {
        std::string input;
        store.get(input, ':');
        std::istringstream str(input);
        std::string temp;
        str >> temp;
        if (fieldName)
            *fieldName = temp;

        std::uint32_t temp2{0};

        input.clear();
        store.get(input, '\n');

        std::istringstream str2(input);
        str2 >> temp2;

        if (value)
            *value = temp2;
    }
}
