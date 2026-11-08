//
// Created by Tony Horrobin on 18/10/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Atom.h"
#include "core/Variant.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dagbase
{
    class InputStream;
    class OutputStream;

    enum SchemaVersionFlags : std::uint8_t
    {
        VERSION_NONE = 0,
        //! This is an alpha release
        VERSION_ALPHA = 1,
        //! This is a beta release
        VERSION_BETA = 2,
        //! This is a release candidate:The top four bits give the RC number in [0-15]
        VERSION_RC = 3
    };

    enum SchemaVersionMasks : std::uint8_t
    {
        MASK_FLAGS = 0x03,
        MASK_RC = 0x3f<<2,
    };

    enum SchemaVersionShifts : std::uint8_t
    {
        SHIFT_FLAGS = 0,
        SHIFT_RC = 2
    };

    const char DAGBASE_API *  schemaVersionFlagsToString(SchemaVersionFlags value);
    SchemaVersionFlags DAGBASE_API parseSchemaVersionFlags(const char* str);

    struct DAGBASE_API SchemaVersion
    {
        std::uint8_t major{0};
        std::uint8_t minor{0};
        std::uint8_t patch{0};
        std::uint8_t flags{VERSION_NONE};

        //! Make a flags field using the flags and rc components
        //! \param[in] flags : SchemaVersionFlags The flags component:
        //! \see SchemaVersionFlags
        //! \param[in] rc : uint8 The release candidate, which will be OR-ed into the top four bits as
        //! defined by SchemaVersionMasks and SchemaVersionShifts
        //  \note rc must be in [0,15] to fit in the available space
        //! \return a SchemaVersion comprised of the two components
        //! \retval a valid SchemaVersionFlags if flags and rc are valid
        //! \retval a default SchemaVersionFlags otherwise
        static std::uint8_t makeFlags(SchemaVersionFlags flags, std::uint8_t rc);

        bool operator==(const SchemaVersion& other) const
        {
            return major == other.major && minor == other.minor && patch == other.patch && flags == other.flags;
        }

        OutputStream& writeToStream(OutputStream& str) const;

        InputStream& readFromStream(InputStream& str);

        std::string toString() const;
    };

    struct DAGBASE_API SchemaHeader
    {
        std::uint32_t magic{0xdeadbeef};
        SchemaVersion version{0,0,1,SchemaVersion::makeFlags(VERSION_NONE, 0)};

        OutputStream& writeToStream(OutputStream& str) const;

        InputStream& readFromStream(InputStream& str);

        bool operator==(const SchemaHeader& other) const
        {
            return magic == other.magic && version == other.version;
        }
    };

    //! 64bit FNV-1a hash
    using SchemaIdentifier = std::uint64_t;

    struct DAGBASE_API SchemaSemantics
    {
        SchemaIdentifier id{0};
        Atom name;
        Atom symbol;
        Atom dimension;
        Variant minValue;
        Variant maxValue;
        //! \note Encoded as a variable-length integer
        std::uint32_t minLength{0};
        //! \note Encoded as a variable-length integer
        std::uint32_t maxLength{std::numeric_limits<std::uint32_t>::max()};
        std::string validation;
    };

    struct DAGBASE_API SchemaType
    {
        SchemaIdentifier id{0};
        Atom name;
        SchemaIdentifier layout{0};
        SchemaSemantics semantic;
    };

    struct DAGBASE_API Schema
    {
        //! Number of types in the Schema
        //! \note This will be encoded as a variable-length integer in the stream.
        std::uint32_t numTypes{0};
        std::vector<SchemaType> types;
    };
}