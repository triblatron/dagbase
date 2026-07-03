//
// Created by tony on 03/02/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include "Types.h"
#include "util/DebugPrinter.h"

#include <string>
#include <cstdint>

namespace dagbase
{
    class InputStream;
    class OutputStream;
    class DebugPrinter;

    struct DAGBASE_API MetaPort
    {
        enum Flags : std::uint32_t
        {
            FLAGS_NONE    = 0,
            FLAGS_OWN_BIT = 1<<0
        };

        Flags flags{FLAGS_OWN_BIT};

        MetaPort() = default;

        MetaPort(bool isOwned)
        {
            setIsOwned(isOwned);
        }

        MetaPort(const MetaPort& other)
        {
            setIsOwned(other.isOwned());
        }

        void setFlag(Flags flag)
        {
            flags = static_cast<Flags>(flags | flag);
        }

        void clearFlag(Flags flag)
        {
            flags = static_cast<Flags>(flags & ~flag);
        }

        bool isFlagSet(Flags flag) const
        {
            return static_cast<bool>(flags & flag);
        }

        void setIsOwned(bool isOwned)
        {
            if (isOwned)
            {
                setFlag(FLAGS_OWN_BIT);
            }
            else
            {
                clearFlag(FLAGS_OWN_BIT);
            }
        }

        bool isOwned() const
        {
            return isFlagSet(FLAGS_OWN_BIT);
        }

        bool operator==(const MetaPort& other) const;

        dagbase::InputStream& read(dagbase::InputStream& str);

        dagbase::OutputStream& write(dagbase::OutputStream& str) const;

        void debug(dagbase::DebugPrinter& printer) const;

        std::ostream& toLua(std::ostream& str) const;

        static std::string flagsToString(Flags flags);

        static Flags parseFlags(std::string const& str);
    };
}
