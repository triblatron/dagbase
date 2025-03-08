//
// Created by tony on 12/03/24.
//

#include "config/config.h"

#include "util/DebugPrinter.h"

#include <iostream>

namespace dagbase
{
    DebugPrinter &DebugPrinter::println(std::string line)
    {
        if (_str != nullptr)
        {
            printIndent();
            (*_str) << line;
            (*_str) << '\n';
        }

        return *this;
    }

    DebugPrinter& DebugPrinter::printIndent()
    {
        for (std::uint32_t i=0; i<_indentLevel; ++i)
        {
            for (std::uint32_t j=0; j<_spacesPerIndent; ++j)
            {
                (*_str) << ' ';
            }
        }

        return *this;
    }

}