//
// Created by Tony Horrobin on 18/10/2026.
//

#include "config/config.h"

#include "core/Schema.h"

#include "util/enums.h"

#include <sstream>
#include <cassert>

#include "io/InputStream.h"
#include "io/OutputStream.h"

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

    OutputStream & SchemaVersion::writeToStream(OutputStream &str) const
    {
        str.writeHeader("SchemaVersion");
        str.writeField("major");
        str.writeUInt8(major);
        str.writeField("minor");
        str.writeUInt8(minor);
        str.writeField("patch");
        str.writeUInt8(patch);
        str.writeField("flags");
        str.writeUInt8(flags);
        str.writeFooter();

        return str;
    }

    InputStream & SchemaVersion::readFromStream(InputStream &str)
    {
        std::string className, fieldName;
        str.readHeader(&className);
        str.readField(&fieldName);
        str.readUInt8(&major);
        str.readField(&fieldName);
        str.readUInt8(&minor);
        str.readField(&fieldName);
        str.readUInt8(&patch);
        str.readField(&fieldName);
        str.readUInt8(&flags);
        str.readFooter();

        return str;
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

    OutputStream & Header::writeToStream(OutputStream &str) const
    {
        str.writeHeader("Header");
        str.writeField("magic");
        str.writeUInt32(magic);
        str.writeField("version");
        version.writeToStream(str);
        str.writeFooter();

        return str;
    }

    InputStream & Header::readFromStream(InputStream &str)
    {
        std::string className, fieldName;
        str.readHeader(&className);
        str.readField(&fieldName);
        str.readUInt32(&magic);
        str.readField(&fieldName);
        version.readFromStream(str);
        str.readFooter();

        return str;
    }
}
