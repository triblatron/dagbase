//
// Created by Tony Horrobin on 27/04/2025.
//

#include "config/config.h"

#include "core/MetaProperty.h"
#include "util/enums.h"
#include "util/Searchable.h"

namespace dagbase
{

    void MetaProperty::configure(ConfigurationElement &config)
    {
        if (auto element = config.findElement("dataType"); element)
        {
            _dataType = parseDataType(element->asString().c_str());
        }

        if (auto element = config.findElement("format"); element)
        {
            _format = parseFormat(element->asString().c_str());
        }
    }

    ConfigurationElement::ValueType MetaProperty::find(std::string_view path) const
    {
        ConfigurationElement::ValueType retval;

        retval = findEndpoint(path, "dataType", dataTypeToString(_dataType));
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "format", formatToString(_format));
        if (retval.has_value())
            return retval;

        return {};
    }

    const char *MetaProperty::dataTypeToString(MetaProperty::DataType value)
    {
        switch (value)
        {
            ENUM_NAME(DATATYPE_UNKNOWN)
            ENUM_NAME(DATATYPE_STRING)
            ENUM_NAME(DATATYPE_INT)
            ENUM_NAME(DATATYPE_BOOL)
            ENUM_NAME(DATATYPE_DOUBLE)
        }
        return "<error>";
    }

    MetaProperty::DataType MetaProperty::parseDataType(const char *str)
    {
        TEST_ENUM(DATATYPE_UNKNOWN, str);
        TEST_ENUM(DATATYPE_STRING, str);
        TEST_ENUM(DATATYPE_INT, str);
        TEST_ENUM(DATATYPE_BOOL, str);
        TEST_ENUM(DATATYPE_DOUBLE, str);

        return MetaProperty::DATATYPE_UNKNOWN;
    }

    const char *MetaProperty::formatToString(MetaProperty::Format value)
    {
        switch (value)
        {
            ENUM_NAME(FORMAT_UNKNOWN)
            ENUM_NAME(FORMAT_NONE)
            ENUM_NAME(FORMAT_COLOUR)
        }

        return "<error>";
    }

    MetaProperty::Format MetaProperty::parseFormat(const char *str)
    {
        TEST_ENUM(FORMAT_UNKNOWN, str);
        TEST_ENUM(FORMAT_NONE, str);
        TEST_ENUM(FORMAT_COLOUR, str);

        return MetaProperty::FORMAT_UNKNOWN;
    }
}