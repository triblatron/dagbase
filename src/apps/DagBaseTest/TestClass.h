//
// Created by Tony Horrobin on 18/04/2025.
//

#ifndef DAGUI_TESTCLASS_H
#define DAGUI_TESTCLASS_H

#include "core/DynamicVisitor.h"

struct TestClass
{
    int numCalls{0};

    TestClass()
            :
            numCalls(0)
    {
        // Do nothing.
    }

    dagbase::Atom typeName() const
    {
        return dagbase::Atom::intern("TestClass");
//        return "TestObject";
    }

    void accept(dagbase::DynamicVisitor<TestClass>& visitor)
    {
        visitor.visit(*this);
    }
};

#endif //DAGUI_TESTCLASS_H
