//
// Created by tony on 03/02/24.
//

#include "config/config.h"

#include "core/MetaPort.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"
#include "util/DebugPrinter.h"
#include "util/enums.h"

#include <iostream>

namespace dagbase
{
    dagbase::OutputStream& MetaPort::write(dagbase::OutputStream& str) const
    {
        str.writeHeader("MetaPort");
        str.writeField("flags");
        str.writeUInt32(flags);
        str.writeFooter();

        return str;
    }

    dagbase::InputStream &MetaPort::read(dagbase::InputStream &str)
    {
        std::string className;
        std::string fieldName;
        str.readHeader(&className);
        str.readField(&fieldName);
        std::uint32_t rawFlags;
        str.readUInt32(&rawFlags);
        flags = static_cast<Flags>(rawFlags);
        str.readFooter();

        return str;
    }

    bool MetaPort::operator==(const MetaPort &other) const
    {
        if (flags != other.flags)
            return false;

        return true;
    }

    void MetaPort::debug(dagbase::DebugPrinter& printer) const
    {
        printer.println("flags: " + flagsToString(flags));
    }

    DebugPrinter& MetaPort::toLua(DebugPrinter& printer) const
    {
        printer.printIndent().print("flags = \"").print(flagsToString(flags)).print("\",\n");

        return printer;
    }

    std::string MetaPort::flagsToString(Flags flags)
    {
        std::string retval;

        if (flags == FLAGS_NONE)
            return "FLAGS_NONE";

        BIT_NAME(flags, FLAGS_OWN_BIT, retval);

        if (!retval.empty() && retval.back() == ' ')
            retval.pop_back();

        return retval;
    }

    MetaPort::Flags MetaPort::parseFlags(std::string const &str)
    {
        Flags value{FLAGS_NONE};

        TEST_BIT(FLAGS_OWN_BIT, str, value);

        return value;
    }
}
