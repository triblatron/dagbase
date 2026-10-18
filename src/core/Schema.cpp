//
// Created by Tony Horrobin on 18/10/2026.
//

#include "config/config.h"

#include "core/Schema.h"

#include "util/enums.h"

#include <sstream>
#include <cassert>

namespace dagbase
{
    const char* schemaVersionFlagsToString(SchemaVersionFlags value)
    {
        switch (value)
        {
            ENUM_NAME(VERSION_NONE)
            ENUM_NAME(VERSION_ALPHA)
            ENUM_NAME(VERSION_BETA)
            ENUM_NAME(VERSION_RC)
        }

        return "<error>";
    }

    SchemaVersionFlags parseSchemaVersionFlags(const char* str)
    {
        SchemaVersionFlags value{VERSION_NONE};

        TEST_ENUM(VERSION_ALPHA, str)
        TEST_ENUM(VERSION_BETA, str)
        TEST_ENUM(VERSION_RC, str)

        return value;
    }

    std::uint8_t SchemaVersion::makeFlags(SchemaVersionFlags flags, std::uint8_t rc)
    {
        std::uint8_t retval = flags | ((rc<<SHIFT_RC)&MASK_RC);

        return retval;
    }

    std::string SchemaVersion::toString() const
    {
        std::ostringstream str;

        str << int(major) << '.' << int(minor) << '.' << int(patch);
        auto prerelease = flags & MASK_FLAGS>>SHIFT_FLAGS;
        if (prerelease!= VERSION_NONE)
        {
            str << '-';
            switch (prerelease)
            {
                case VERSION_ALPHA:
                    str << "alpha";
                    break;
                case VERSION_BETA:
                    str << "beta";
                    break;
                case VERSION_RC:
                    str << "rc";
                    break;
                default:
                    // We should not get into here.
                    assert(false);
                    break;
            }
            std::uint8_t rc = (flags & MASK_RC)>>SHIFT_RC;
            if (rc!=0)
            {
                str << '.' << int(rc);
            }
        }
        return str.str();
    }
}
