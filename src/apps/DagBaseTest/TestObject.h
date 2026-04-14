//
// Created by Tony Horrobin on 18/04/2025.
//

#ifndef DAGUI_TESTOBJECT_H
#define DAGUI_TESTOBJECT_H

#include "core/Class.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"

struct TestObject : public dagbase::Class
{
    std::uint32_t value{0};

    const char* className() const override
    {
        return "TestObject";
    }

    dagbase::OutputStream& writeToStream(dagbase::OutputStream& format, dagbase::NodeLibrary& nodeLib, dagbase::Lua& lua) const override
    {
        format.writeHeader("TestObject");
        Class::writeToStream(format, nodeLib, lua);
        format.writeField("value");// << value << " }\n";
        format.writeUInt32(value);
        format.writeFooter();

        return format;
    }

    dagbase::InputStream& readFromStream(dagbase::InputStream& format, dagbase::NodeLibrary& nodeLib, dagbase::Lua& lua) override
    {
        std::string className;
        format.readHeader(&className);
        if (className != "TestObject")
            return format;
        Class::readFromStream(format, nodeLib, lua);
        std::string fieldName;
        format.readField(&fieldName);
        if (fieldName != "value")
            return format;
        format.readUInt32(&value);
        format.readFooter();

        return format;
    }
};

#endif //DAGUI_TESTOBJECT_H
