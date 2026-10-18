//
// Created by Tony Horrobin on 18/10/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include <cstdint>
#include <string>

namespace dagbase
{
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
        MASK_FLAGS = 0xf,
        MASK_RC = 0xf<<4
    };

    enum SchemaVersionShifts : std::uint8_t
    {
        SHIFT_FLAGS = 0,
        SHIFT_RC = 4
    };

    const char* DAGBASE_API schemaVersionFlagsToString(SchemaVersionFlags value);
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

        std::string toString() const;
    };

    struct DAGBASE_API Header
    {
        std::uint32_t magic{0xdeadbeef};
        SchemaVersion version;
    };
}