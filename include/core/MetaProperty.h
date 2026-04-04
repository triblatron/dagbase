//
// Created by Tony Horrobin on 27/04/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/ConfigurationElement.h"

#include <string_view>
#include <string>

namespace dagbase
{
    class DAGBASE_API MetaProperty
    {
    public:
        //! Base data type for a property
        enum DataType : std::uint32_t
        {
            DATATYPE_UNKNOWN,
            DATATYPE_STRING,
            DATATYPE_INT,
            DATATYPE_BOOL,
            DATATYPE_DOUBLE
        };

        enum Format : std::uint32_t
        {
            FORMAT_UNKNOWN,
            FORMAT_NONE,
            FORMAT_COLOUR
        };

        enum Flags : std::uint32_t
        {
            FLAGS_NONE,
            FLAGS_READ_BIT = 1<<0,
            FLAGS_WRITE_BIT = 1<<1,
        };
    public:
        void configure(ConfigurationElement& config);

        ConfigurationElement::ValueType find(std::string_view path) const;

        static const char* dataTypeToString(DataType value);
        static DataType parseDataType(const char* str);

        static const char* formatToString(Format value);
        static Format parseFormat(const char* str);

        static std::string flagsToString(Flags value);
        static Flags parseFlags(std::string_view str);
    private:
        std::string _name;
        DataType _dataType{DATATYPE_UNKNOWN};
        Format _format{FORMAT_UNKNOWN};
        Flags _flags{FLAGS_READ_BIT};
    };
}
