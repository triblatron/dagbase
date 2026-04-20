//
// Created by Tony Horrobin on 13/12/2025.
//

#include "core/Signal.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"
#include "core/Function.h"

#include <gtest/gtest.h>
#include <sstream>

#include "TestObject.h"
#include "config/macos_config.h"
#include "core/MetaClass.h"
#include "core/ClassDescription.h"
#include "util/VectorMap.h"

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

struct Type;

struct Field
{
    Type* type{nullptr};
};

struct Method
{
    std::uint32_t arity{0};
    Type* returnType{nullptr};
    std::vector<Type> arguments;
};

struct Member
{
    dagbase::Atom name;
    std::variant<Field, Method> data;
};

struct Type
{
    std::vector<Member> members;
    std::size_t size{0};
    bool complete{false};
};

class TypeRegistry
{
public:
    ~TypeRegistry() = default;

    static TypeRegistry& getTypeRegistry()
    {
        static TypeRegistry registry;

        return registry;
    }

    void registerType(dagbase::Atom name, Type* type);

    void unregisterType(dagbase::Atom name);

    Type* findType(dagbase::Atom name);
private:
    dagbase::VectorMap<dagbase::Atom, Type*> _types;
};

template<typename T>
struct MetaClassRegistration
{
    MetaClassRegistration();
    ~MetaClassRegistration();
};

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

    static MetaClassRegistration<TestEmitter> registration;
private:
    std::int32_t _test{0};
};

struct Int32
{
    static Type& getType();
};


Type& Int32::getType()
{
    static Type type{{}, sizeof(std::int32_t), true};

    return type;
}

template<>
MetaClassRegistration<Int32>::MetaClassRegistration()
{
    TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("int32"), &Int32::getType());
}

template<>
MetaClassRegistration<TestEmitter>::MetaClassRegistration()
{
    static Type type{};
    static bool inited = false;
    if (!inited)
    {
        std::cout << "MetaClassRegistration<TestEmitter>::MetaClassRegistration()" << std::endl;
        type.size = sizeof(TestEmitter);
        Member test;
        test.name = dagbase::Atom::intern("test");
        test.data = Field();
        std::get<0>(test.data).type = &Int32::getType();
        type.members.emplace_back(test);
        type.complete = true;
        TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("TestEmitter"), &type);
        inited = true;
    }
}

template<>
MetaClassRegistration<TestEmitter>::~MetaClassRegistration()
{
    std::cout << "MetaClassRegistration<TestEmitter>::~MetaClassRegistration()" << std::endl;
    TypeRegistry::getTypeRegistry().unregisterType(dagbase::Atom::intern("TestEmitter"));
}

void TypeRegistry::registerType(dagbase::Atom name, Type* type)
{
    _types.insert(dagbase::VectorMap<dagbase::Atom, Type*>::value_type(name, type));
}

void TypeRegistry::unregisterType(dagbase::Atom name)
{
    if (auto it=_types.find(name); it!=_types.end())
        _types.erase(it);
}

Type * TypeRegistry::findType(dagbase::Atom name)
{
    if (auto it=_types.find(name); it!=_types.end())
        return it->second;

    return nullptr;
}

MetaClassRegistration<TestEmitter> registration;

class B;

class A
{
public:
    A() = default;

    static Type& getType();

    static MetaClassRegistration<A> registration;
private:
    A* _parent{nullptr};
};

template<>
MetaClassRegistration<A>::MetaClassRegistration()
{
    Type& type = A::getType();
    TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("A"), &type);
}

template<>
MetaClassRegistration<A>::~MetaClassRegistration()
{
    std::cout << "MetaClassRegistration<A> unregistering" << std::endl;
    TypeRegistry::getTypeRegistry().unregisterType(dagbase::Atom::intern("A"));
}

Type & A::getType()
{
    // Use the Meyer Singleton pattern to create a static object in a thread-safe manner.
    static Type type{};
    static bool inited = false;
    if (!inited)
    {
        std::cout << "MetaClassRegistration<A> registering" << std::endl;
        type.size = sizeof(A);
        type.members.emplace_back();
        type.members[0].name = dagbase::Atom::intern("parent");
        type.members[0].data = Field();
        std::get<0>(type.members[0].data).type = &type;
        type.complete = true;
        inited = true;
    }

    return type;
}

MetaClassRegistration<A> A::registration;

class C;

class B
{
public:
    B() = default;

    static Type& getType(bool complete=true);
    static MetaClassRegistration<B> registration;
private:
    C* _c{nullptr};
};

template<>
MetaClassRegistration<B>::MetaClassRegistration()
{
    Type& type = B::getType();
    TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("B"), &type);
}

template<>
MetaClassRegistration<B>::~MetaClassRegistration()
{
    std::cout << "MetaClassRegistration<B> unregistering" << std::endl;
    TypeRegistry::getTypeRegistry().unregisterType(dagbase::Atom::intern("B"));
}

MetaClassRegistration<B> B::registration;

class C
{
public:
    C() = default;

    static Type& getType(bool complete=true);
    static MetaClassRegistration<C> registration;
private:
    B* _b{nullptr};
};

Type& B::getType(bool complete)
{
    static Type type{};
    static bool inited = false;

    if (!complete)
        return type;

    if (!inited)
    {
        type.size = sizeof(B);
        type.members.emplace_back();
        type.members[0].name = dagbase::Atom::intern("c");
        type.members[0].data = Field();
        std::get<0>(type.members[0].data).type = &C::getType(false);
        type.complete = true;
        inited = true;
    }

    return type;
}

Type & C::getType(bool complete)
{
    static Type type{};
    static bool inited = false;

    if (!complete)
        return type;

    if (!inited)
    {
        type.size = sizeof(C);
        type.members.emplace_back();
        type.members[0].name = dagbase::Atom::intern("b");
        type.members[0].data = Field();
        std::get<0>(type.members[0].data).type = &B::getType(false);
        type.complete = true;
        inited = true;
    }

    return type;
}

template<>
MetaClassRegistration<C>::MetaClassRegistration()
{
    Type& type = C::getType();

    TypeRegistry::getTypeRegistry().registerType(dagbase::Atom::intern("C"), &type);
}

template<>
MetaClassRegistration<C>::~MetaClassRegistration()
{
    std::cout << "MetaClassRegistration<C> unregistering" << std::endl;
    TypeRegistry::getTypeRegistry().unregisterType(dagbase::Atom::intern("C"));
}

MetaClassRegistration<C> C::registration;

class TypeRegistry_testTypeRegistration : public ::testing::TestWithParam<std::tuple<dagbase::Atom>>
{

};

TEST_P(TypeRegistry_testTypeRegistration, testRegistration)
{
    auto name = std::get<0>(GetParam());
    auto actual = TypeRegistry::getTypeRegistry().findType(name);

    ASSERT_NE(nullptr, actual);
    ASSERT_TRUE(actual->complete);
}

INSTANTIATE_TEST_SUITE_P(TypeRegistry, TypeRegistry_testTypeRegistration,::testing::Values(
    std::make_tuple(dagbase::Atom::intern("TestEmitter")),
    std::make_tuple(dagbase::Atom::intern("A")),
    std::make_tuple(dagbase::Atom::intern("B")),
    std::make_tuple(dagbase::Atom::intern("C"))
    ));