//
// Created by tony on 03/02/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include "Types.h"
#include "util/DebugPrinter.h"

#include <string>


namespace dagbase
{
    class InputStream;
    class OutputStream;
    class DebugPrinter;

    struct DAGBASE_API MetaPort
    {
        bool isOwned{true};

        MetaPort() = default;

        MetaPort(bool isOwned)
        :
        isOwned(isOwned)
        {
            // Do nothing.
        }

        MetaPort(const MetaPort& other)
        {
            isOwned = other.isOwned;
        }

        explicit MetaPort(dagbase::InputStream& str);

        bool operator==(const MetaPort& other) const;

        dagbase::InputStream& read(dagbase::InputStream& str);

        dagbase::OutputStream& write(dagbase::OutputStream& str) const;

        void debug(dagbase::DebugPrinter& printer) const;

        std::ostream& toLua(std::ostream& str) const;
    };
}
