//
// Created by Tony Horrobin on 27/04/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/ConfigurationElement.h"

#include <string_view>
#include <string>
#include <cstdint>

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
    public:
        void configure(ConfigurationElement& config);

        ConfigurationElement::ValueType find(std::string_view path) const;

        static const char* dataTypeToString(DataType value);
        static DataType parseDataType(const char* str);

        static const char* formatToString(Format value);
        static Format parseFormat(const char* str);
    private:
        std::string _name;
        DataType _dataType{DATATYPE_UNKNOWN};
        Format _format{FORMAT_UNKNOWN};
    };
}
