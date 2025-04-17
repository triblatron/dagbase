//
// Created by Tony Horrobin on 17/04/2025.
//

#include "core/DynamicVisitor.h"

#include <gtest/gtest.h>

//class DynamicVisitor_testVisit : public ::testing::TestWithParam<std::tuple<>>
//{
//public:
//    void SetUp() override
//    {
//        dagbase::Atom::reset();
//    }
//};

struct TestClass
{
    int numCalls{0};

    TestClass()
    :
    numCalls(0)
    {
        // Do nothing.
    }

    std::string typeName() const
    {
//        return dagbase::Atom::intern("TestObject");
        return "TestObject";
    }

    void accept(dagbase::DynamicVisitor<TestClass>& visitor)
    {
        visitor.visit(*this);
    }
};

TEST(DynamicVisitor_testVisit, testExpectedCall)
{
//    dagbase::Atom::reset();
    dagbase::DynamicVisitor<TestClass> sut;
    TestClass obj;
    sut.registerHandler(obj.typeName(), [](TestClass& obj) {
        obj.numCalls++;
    });
//    sut.registerHandler("TestObject",1);
    obj.accept(sut);
    EXPECT_EQ(1,obj.numCalls);
}

//INSTANTIATE_TEST_SUITE_P(DynamicVisitor, DynamicVisitor_testVisit, ::testing::Values());