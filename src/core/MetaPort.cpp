//
// Created by tony on 03/02/24.
//

#include "config/config.h"

#include "core/MetaPort.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"
#include "util/DebugPrinter.h"
#include <iostream>

namespace dagbase
{
    MetaPort::MetaPort(dagbase::InputStream &str)
    {
        std::string className;
        std::string fieldName;
        str.readHeader(&className);

        str.addObj(this);
        str.readField(&fieldName);
        str.readBool(&isOwned);
        str.readFooter();
    }

    dagbase::OutputStream& MetaPort::write(dagbase::OutputStream& str) const
    {
        str.writeHeader("MetaPort");
        str.writeField("isOwned");
        str.writeBool(isOwned);
        str.writeFooter();

        return str;
    }

    dagbase::InputStream &MetaPort::read(dagbase::InputStream &str)
    {
        std::string className;
        std::string fieldName;
        str.readHeader(&className);
        str.readField(&fieldName);
        str.readBool(&isOwned);
        str.readFooter();

        return str;
    }

    bool MetaPort::operator==(const MetaPort &other) const
    {
        if (isOwned != other.isOwned)
            return false;

        return true;
    }

    void MetaPort::debug(dagbase::DebugPrinter& printer) const
    {
        printer.println("isOwned: " + std::to_string(isOwned));
    }

    std::ostream &MetaPort::toLua(std::ostream &str) const
    {
        str << "isOwned = " << isOwned;

        return str;
    }
}
