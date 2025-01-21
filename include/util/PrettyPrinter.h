#pragma once

#include "config/DagBaseExport.h"

#include <iosfwd>
#include <string>

namespace dagbase
{
    //! \class PrettyPrinter
    //! An aid to debugging node graphs
    //! Supports indentation and printing lines.
    class DAGBASE_API PrettyPrinter
    {
    public:
        PrettyPrinter(std::ostream & str, int tabSize)
        :
        _str(str),
        _indent(0),
        _tabSize(tabSize)
        {
            // Do nothing.
        }

        //! Increase indentation by one.
        PrettyPrinter & indent()
        {
            _indent++;

            return *this;
        }

        //! Decrease indentation by one.
        PrettyPrinter & outdent()
        {
            _indent--;

            return *this;
        }

        //! \return The current indentation level.
        [[nodiscard]]int indentation() const
        {
            return _indent;
        }

        //! \return The number of spaces per indentation level.
        [[nodiscard]]int tabSize() const
        {
            return _tabSize;
        }

        //! Print a line indented by the indentation level * tabSize spaces.
        PrettyPrinter & println(const std::string & line);

        PrettyPrinter & printIndent();

        std::ostream & str()
        {
            return _str;
        }
    private:
        std::ostream & _str;
        int _indent;
        int _tabSize;
    };

    template<class T>
    PrettyPrinter & operator<<(PrettyPrinter & printer, const T & value)
    {
        printer.str() << value;

        return printer;
    }
}
