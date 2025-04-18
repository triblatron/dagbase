//
// Created by Tony Horrobin on 18/04/2025.
//

#ifndef DAGUI_TESTOBJECT_H
#define DAGUI_TESTOBJECT_H

#include "core/Class.h"

struct TestObject : public dagbase::Class
{
    std::uint32_t value{0};

    const char* className() const override
    {
        return "TestObject";
    }

    void writeToStream(dagbase::StreamFormat& format) const override
    {
        format.writeHeader("TestObject");
        Class::writeToStream(format);
        format.writeField("value");// << value << " }\n";
        format.writeUInt32(value);
        format.writeFooter();
    }

    void readFromStream(dagbase::StreamFormat& format) override
    {
        std::string className;
        format.readHeader(&className);
        if (className != "TestObject")
            return;
        Class::readFromStream(format);
        std::string fieldName;
        format.readField(&fieldName);
        if (fieldName != "value")
            return;
        format.readUInt32(&value);
        format.readFooter();
        // std::string className;
        //
        // str >> className;
        // if (className!="TestObject")
        //     return;
        // char temp;
        // str >> temp;
        // if (temp!='{')
        //     return;
        // Class::readFromStream(str);
        // std::string fieldName;
        // str >> fieldName;
        // if (fieldName!="value:")
        //     return;
        // str >> value;
        // str >> temp;
        // if (temp!='}')
        //     return;
    }
};

#endif //DAGUI_TESTOBJECT_H
