//
// Created by Tony Horrobin on 13/12/2025.
//

#include "core/Signal.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"
#include "core/Function.h"
#include "util/enums.h"

#include "config/macos_config.h"
#include "core/MetaClass.h"
#include "core/ClassDescription.h"
#include "util/VectorMap.h"
#include "core/TypeRegistry.h"

#include "TestObject.h"
#include "A.h"
#include "B.h"

#include <gtest/gtest.h>
#include <sstream>
#include <cstdint>

#include "test/TestUtils.h"

class SignalSlot_testInvoke : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

struct TestSlot
{
    TestSlot() = default;;

    void configure(dagbase::ConfigurationElement &config)
    {
        dagbase::ConfigurationElement::readConfig(config, "result", &expectedResult);
    }

    dagbase::Variant operator()()
    {
        result = expectedResult;
        return result;
    }

    void makeItSo()
    {
        EXPECT_EQ(expectedResult, result);
    }

    dagbase::Variant expectedResult;
    dagbase::Variant result;
};

TEST_P(SignalSlot_testInvoke, testExpectedResult)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::Signal<dagbase::Variant()> testSignal;
    TestSlot testSlot = {};
    testSlot.configure(*config);
    testSignal.connect(testSlot);


    auto actualResult = testSignal();
    EXPECT_EQ(testSlot.expectedResult, actualResult);
}

INSTANTIATE_TEST_SUITE_P(SignalSlot, SignalSlot_testInvoke, ::testing::Values(
    std::make_tuple("data/tests/SignalSlot/Simple.lua")
    ));

class SignalSlot_testLuaSlot : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

struct LuaSlot
{
    void configure(dagbase::ConfigurationElement &config)
    {
        dagbase::Variant value;
        dagbase::ConfigurationElement::readConfig(config, "slot", &value);
        _func = value.asFunction(nullptr);
    }

    ~LuaSlot()
    {
        delete _func;
    }

    void operator()()
    {
        if (_func) {
            (*_func)(0,0);
        }
    }

    dagbase::Function* _func{nullptr};
};

TEST_P(SignalSlot_testLuaSlot, testExpectedResult)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    LuaSlot testSlot = {};
    testSlot.configure(*config);
    testSlot();
    EXPECT_TRUE(lua.tableExists("result"));
}

INSTANTIATE_TEST_SUITE_P(SignalSlot, SignalSlot_testLuaSlot, ::testing::Values(
    std::make_tuple("data/tests/SignalSlot/LuaSlot.lua")
    ));

int standaloneSlot()
{
   return 1;
}

TEST(SignalSlot, SignalSlot_testStandaloneFunction)
{
    dagbase::Signal<int()> testSignal;
    testSignal.connect(standaloneSlot);
    EXPECT_EQ(1,testSignal());
}

class MemberFunctionSlot
{
public:
    int slot()
    {
        return 1;
    }
};

TEST(SignalSlot, SignalSlot_testMemberFunction)
{
    dagbase::Signal<int()> testSignal;
    MemberFunctionSlot testSlot;
    testSignal.connect([&testSlot]() {
        return testSlot.slot();
    });
    EXPECT_EQ(1,testSignal());
}

TEST(SignalSlot, SignalSlot_testMultipleVoidSlots)
{
    dagbase::Signal<void()> testSignal;
    int i{0};
    int j{0};
    testSignal.connect([&i]() {
        i=1;
    });
    testSignal.connect([&j]() {
        j=2;
    });
    testSignal();
    EXPECT_EQ(1,i);
    EXPECT_EQ(2,j);
}

TEST(SignalSlot, SignalSlot_testMultipleReturnSlots)
{
    dagbase::Signal<int()> testSignal;
    testSignal.connect([&testSignal]() {
        return 1;
    });
    testSignal.connect([&testSignal]() {
        return 2;
    });
    EXPECT_EQ(2, testSignal());
}

template<typename T>
struct Maximum
{
    template<typename InputIterator, typename... Args>
    T operator()(InputIterator first, InputIterator last, Args&&... args)
    {
        if (first == last)
            return T();
        T maxSoFar = (first->second)(std::forward<Args>(args)...);
        ++first;
        for (auto it = first; it != last; ++it)
        {
            T result = (it->second)(std::forward<Args>(args)...);
            if (maxSoFar < result)
                maxSoFar = result;
        }

        return maxSoFar;
    }
};

TEST(SignalSlot, SignalSlot_testCombiner)
{
    dagbase::Signal<int(),Maximum<int>> testSignal;
    testSignal.connect([&testSignal]() {
        return 10;
    });
    testSignal.connect([&testSignal]() {
        return 1;
    });

    EXPECT_EQ(10, testSignal());
}

TEST(SignalSlot, SignalSlot_testDisconnect)
{
    dagbase::Signal<int()> testSignal;
    auto slot = [&testSignal]() {
        return 1;
    };
    std::size_t slotIndex = testSignal.connect(slot);
    testSignal.disconnect(slotIndex);
    auto result = testSignal();
    EXPECT_EQ(0, result);
}

TEST(SignalSlot, SignalSlot_testScopedConnection)
{
    dagbase::Signal<int()> testSignal;
    {
        dagbase::ScopedConnection connection(&testSignal,testSignal.connect([]() {
            return 1;
        }));
        EXPECT_EQ(1,testSignal());
    }
    EXPECT_EQ(0,testSignal());
}

TEST(SignalSlot, SignalSlot_testGroups)
{
    dagbase::Signal<void()> testSignal;
    std::ostringstream str;
    testSignal.connect([&str]() {
        str << " World!";
    }, 1);
    testSignal.connect([&str]() {
        str << "Hello,";
    }, 0);
    testSignal();
    EXPECT_EQ("Hello, World!", str.str());
}

// class SignalSlot_testDisconnectOnDestroySubscriber : public ::testing::TestWithParam<std::tuple<>>
// {
//
// };

struct TestSubscriber
{
    ~TestSubscriber()
    {
        if (index!=~0U)
            testSignal->disconnect(index);
    }

    void setValue(int i)
    {
        _i = i;
    }

    int value() const
    {
        return _i;
    }
    std::size_t index{~0U};
    dagbase::Signal<int()>* testSignal{nullptr};
private:
    int _i{0};
};

class TestPublisher
{
public:
    ~TestPublisher()
    {
    }
    dagbase::Signal<int()> testSignal;
    std::size_t index{~0U};
};

TEST(SignalSlot, SignalSlot_testDisconnectOnDestroySubscriber)
{
    TestPublisher publisher;
    {
        TestSubscriber subscriber;
        subscriber.testSignal = &publisher.testSignal;
        subscriber.index = publisher.testSignal.connect([&subscriber]() { return subscriber.value(); });
        EXPECT_EQ(1, publisher.testSignal.numConnected());
    }
    EXPECT_EQ(0, publisher.testSignal.numConnected());
}

struct TestEnum
{
    enum Enum : std::uint32_t
    {
        TEST_FOO,
        TEST_BAR,
        TEST_BAZ
    };

    static const char* toString(TestEnum::Enum value);

    static Enum parse(const char* str);

    static dagbase::Type& getType();

    static dagbase::MetaClassRegistration<TestEnum> registration;
};

dagbase::MetaClassRegistration<TestEnum> TestEnum::registration(dagbase::Atom::intern("TestEnum"));

class TestEmitter
{
public:
    ~TestEmitter() = default;

    void describe(dagbase::ClassDescription& description) const
    {
        description.addInt32(dagbase::Atom::intern("test"));
    }

    void setTest(std::int32_t value)
    {
        if (_test!=value)
        {
            _test = value;
            // Emit a changed signal
        }
    }

    std::int32_t value() const
    {
        return _test;
    }

    static dagbase::Type& getType();

    static dagbase::MetaClassRegistration<TestEmitter> registration;
private:
    std::int32_t _test{0};
    TestEnum::Enum _enum{ TestEnum::TEST_FOO };
};

using dagbase::Type;
using dagbase::TypeRegistry;

const char* TestEnum::toString(TestEnum::Enum value)
{
    switch (value)
    {
        ENUM_NAME(TEST_FOO)
        ENUM_NAME(TEST_BAR)
        ENUM_NAME(TEST_BAZ)
    }
    return "<error>";
}

TestEnum::Enum TestEnum::parse(const char* str)
{
    TEST_ENUM(TEST_FOO, str);
    TEST_ENUM(TEST_BAR, str);
    TEST_ENUM(TEST_BAZ, str);

    return TestEnum::Enum();
}

dagbase::Type& TestEnum::getType()
{
    static dagbase::Enumeration<TestEnum::Enum> type;
    static bool inited = false;
    
    if (!inited)
    {
        type.size = sizeof(TestEnum::Enum);
        type.toString = &TestEnum::toString;
        type.parse = &TestEnum::parse;
        type.values = {{dagbase::Atom::intern("TEST_FOO"), TEST_FOO}, {dagbase::Atom::intern("TEST_BAR"), TEST_BAR}, {dagbase::Atom::intern("TEST_BAZ"), TEST_BAZ}};
        type.complete = true;
        inited = true;
    }

    return type;
}

dagbase::Type& TestEmitter::getType()
{
    DAGBASE_BEGIN_COMPOUND(TestEmitter)
    DAGBASE_ADD_FIELD(test, dagbase::Int32)
    DAGBASE_ADD_FIELD(anEnum, TestEnum)
    DAGBASE_END_COMPOUND(TestEmitter)

    return type;
}

dagbase::MetaClassRegistration<TestEmitter> registration(dagbase::Atom::intern("TestEmitter"));

class TypeRegistry_testTypeRegistration : public ::testing::TestWithParam<std::tuple<dagbase::Atom, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(TypeRegistry_testTypeRegistration, testRegistration)
{
    auto name = std::get<0>(GetParam());
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actual = TypeRegistry::getTypeRegistry().findType(name);

    ASSERT_NE(nullptr, actual);
    ASSERT_TRUE(actual->complete);

    auto actualValue = actual->find(path);
    if (!actualValue.has_value())
        actualValue = dagbase::findEndpoint(path, "name", dagbase::Variant(name.toString()));
    assertComparison(value, actualValue, tolerance, op);

}

INSTANTIATE_TEST_SUITE_P(TypeRegistry, TypeRegistry_testTypeRegistration,::testing::Values(
    std::make_tuple(dagbase::Atom::intern("TestEmitter"), "name", std::string("TestEmitter"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple(dagbase::Atom::intern("Int32"), "name", std::string("Int32"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple(dagbase::Atom::intern("A"), "name", std::string("A"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple(dagbase::Atom::intern("B"), "name", std::string("B"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple(dagbase::Atom::intern("C"), "name", std::string("C"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple(dagbase::Atom::intern("TestEnum"), "name", std::string("TestEnum"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple(dagbase::Atom::intern("TestEnum"), "TEST_FOO", TestEnum::TEST_FOO, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
    ));
