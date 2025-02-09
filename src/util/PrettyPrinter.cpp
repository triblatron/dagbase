#include "config/config.h"

#include "util/PrettyPrinter.h"

#include <iostream>

namespace dagbase
{
    PrettyPrinter & PrettyPrinter::println(const std::string & line)
    {
        printIndent();
        _str << line << '\n';

        return *this;
    }

    PrettyPrinter & PrettyPrinter::printIndent()
    {
        for (int i=0; i<_indent; ++i)
        {
            for (int j=0; j<_tabSize; ++j)
            {
                _str << " ";
            }
        }

        return *this;
    }
}
