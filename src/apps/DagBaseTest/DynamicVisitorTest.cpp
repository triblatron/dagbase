//
// Created by Tony Horrobin on 17/04/2025.
//

#include "TestClass.h"

#include <gtest/gtest.h>

TEST(DynamicVisitor_testVisit, testExpectedCall)
{
    dagbase::Atom::reset();
    dagbase::DynamicVisitor<TestClass> sut;
    TestClass obj;
    sut.registerHandler(obj.typeName(), [](TestClass& obj) {
        obj.numCalls++;
    });
    obj.accept(sut);
    EXPECT_EQ(1,obj.numCalls);
}
