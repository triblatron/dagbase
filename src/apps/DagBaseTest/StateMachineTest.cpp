//
// Created by Tony Horrobin on 18/12/2025.
//

#include "core/StateMachine.h"
#include "core/HierarchicalStateMachine.h"
#include "test/TestUtils.h"
#include "core/ConfigurationElement.h"
#include "core/Function.h"
#include "core/LuaInterface.h"

#include <gtest/gtest.h>

class StateMachine_testConfigure : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

struct TestState
{
    using Name = dagbase::Atom;
    Name name;
    using Value = std::uint32_t;
    Value value{0};
    bool final{false};

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig(config, "name", &name);
        dagbase::ConfigurationElement::readConfig(config, "value", &value);
        dagbase::ConfigurationElement::readConfig(config, "final", &final);
    }

    bool operator<(const TestState &other) const
    {
        return value < other.value;
    }

    bool operator==(const TestState& other) const
    {
        return name == other.name && value == other.value && final == other.final;
    }
};

struct TestInput
{
    using Name = dagbase::Atom;
    using Value = std::uint32_t;
    Name name;
    Value value{0};

    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig(config, "name", &name);
        dagbase::ConfigurationElement::readConfig(config, "value", &value);
    }

    bool operator<(const TestInput& other) const
    {
        return value < other.value;
    }

    bool operator==(const TestInput& other) const
    {
        return name == other.name && value == other.value;
    }
};

struct TestTransition
{
    dagbase::Atom nextState;

    struct Domain
    {
        dagbase::Atom initialState;
        dagbase::Atom input;

        bool operator<(const Domain& other) const
        {
            return initialState < other.initialState || (initialState == other.initialState && input<other.input);
        }

        bool operator==(const Domain& other) const
        {
            return initialState == other.initialState && input == other.input;
        }

        void configure(dagbase::ConfigurationElement& config)
        {
            dagbase::ConfigurationElement::readConfig(config, "initialState", &initialState);
            dagbase::ConfigurationElement::readConfig(config, "input", &input);
        }
    };

    struct Codomain
    {
        dagbase::Atom nextState;

        void configure(dagbase::ConfigurationElement& config)
        {
            dagbase::ConfigurationElement::readConfig(config, "nextState", &nextState);
        }
    };
};

struct TestEntryExitAction
{
    void configure(dagbase::ConfigurationElement& config)
    {
        _func = config.asFunction();
    }

    dagbase::Variant find(std::string_view path) const
    {
        dagbase::Variant retval;

        retval = dagbase::findEndpoint(path, "numCalls", numCalls);
        if (retval.has_value())
            return retval;

        return {};
    }
    void operator()(TestState& state)
    {
        if (_func)
        {
            (*_func)(0,1);
            ++numCalls;
        }
    }

    std::int64_t     numCalls{0};
    dagbase::Function* _func{nullptr};
};

TEST_P(StateMachine_testConfigure, testExpectedValue)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::StateMachine<TestState, TestTransition, TestInput, TestEntryExitAction> sut;
    sut.configure(*config);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actualValue=sut.find(path);
    assertComparison(value, actualValue, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(StateMachine, StateMachine_testConfigure, ::testing::Values(
        std::make_tuple("data/tests/StateMachine/duplicateState.lua", "numStates", std::uint32_t(2), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/StateMachine/duplicateState.lua", "numInputs", std::uint32_t(1), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/StateMachine/duplicateState.lua", "numTransitions", std::uint32_t(1), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/StateMachine/duplicateState.lua", "numEntryActions", std::uint32_t(1), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/StateMachine/duplicateState.lua", "numExitActions", std::uint32_t(1), 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

class StateMachine_testOnInput : public ::testing::TestWithParam<std::tuple<const char*, const char*>>
{
public:
    using TestStateMachine = dagbase::StateMachine<TestState, TestTransition, TestInput, TestEntryExitAction>;
public:
    void configure(dagbase::ConfigurationElement& config)
    {
        if (auto element = config.findElement("inputs"); element)
        {
            element->eachChild([this](dagbase::ConfigurationElement& child) {
                Input input;

                input.configure(child);

                _inputs.emplace_back(input);

                return true;
            });
        }

        if (auto element=config.findElement("asserts"); element)
        {
            element->eachChild([this](dagbase::ConfigurationElement& child) {
                Assert entry;

                entry.configure(child);
                _asserts.emplace_back(entry);

                return true;
            });


        }
    }

    void makeItSo(TestStateMachine& sut)
    {
        for (auto a : _asserts)
        {
            a.makeItSo(sut);
        }
    }
protected:
    struct Input
    {
        dagbase::Atom input;
        dagbase::Atom nextState;
        bool accepted{false};

        void configure(dagbase::ConfigurationElement& config)
        {
            dagbase::ConfigurationElement::readConfig(config, "input", &input);
            dagbase::ConfigurationElement::readConfig(config, "nextState", &nextState);
            dagbase::ConfigurationElement::readConfig(config, "accepted", &accepted);
        }
    };
    using InputArray = std::vector<Input>;
    InputArray _inputs;
    struct Assert
    {
        dagbase::Atom path;
        dagbase::Variant value;

        void configure(dagbase::ConfigurationElement& config)
        {
            dagbase::ConfigurationElement::readConfig(config, "path", &path);
            dagbase::ConfigurationElement::readConfig(config, "value", &value);
        }

        void makeItSo(TestStateMachine & sut)
        {
            ASSERT_FALSE(path.empty());
            auto actualValue = sut.find(path.value());
            assertComparison(value, actualValue, 0.0, dagbase::ConfigurationElement::RELOP_EQ);// << "Expected " << value << ", got " << actualValue;
        }
    };
    using AssertArray = std::vector<Assert>;
    AssertArray _asserts;
};

TEST_P(StateMachine_testOnInput, testExpectedNextState)
{
    auto testConfigStr = std::get<0>(GetParam());
    dagbase::Lua testLua;
    dagbase::ConfigurationElement* testConfig = nullptr;
    {

        testConfig = dagbase::ConfigurationElement::fromFile(testLua, testConfigStr);
        ASSERT_NE(nullptr, testConfig);
    }
    configure(*testConfig);
    auto configStr = std::get<1>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    TestStateMachine sut;
    sut.configure(*config);
    for (auto& input : _inputs)
    {
        sut.onInput(input.input);
        EXPECT_EQ(input.nextState, sut.state().name);
        EXPECT_EQ(input.accepted, sut.accepted());
    }
    makeItSo(sut);
}

INSTANTIATE_TEST_SUITE_P(StateMachine, StateMachine_testOnInput, ::testing::Values(
        std::make_tuple("data/tests/StateMachine/onTest.lua", "data/tests/StateMachine/duplicateState.lua"),
        std::make_tuple("data/tests/StateMachine/onOneThenTwo.lua", "data/tests/StateMachine/multipleStates.lua")
        ));

class HierarchicalStateMachine_testConfigure : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(HierarchicalStateMachine_testConfigure, testExpectedValue)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::HierarchicalStateMachine sut;
    sut.configure(*config);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actualValue = sut.find(path);
    assertComparison(value, actualValue, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(HierarchicalStateMachine, HierarchicalStateMachine_testConfigure, ::testing::Values(
    std::make_tuple("data/tests/HierarchicalStateMachine/SimpleState.lua", "numStates", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/HierarchicalStateMachine/HierarchicalState.lua", "numStates", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/HierarchicalStateMachine/HierarchicalState.lua", "states.Complex.numStates", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/HierarchicalStateMachine/HierarchicalState.lua", "states.Complex.states.Initial.numStates", std::uint32_t{0}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/HierarchicalStateMachine/HierarchicalState.lua", "currentState", std::int64_t{0}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/HierarchicalStateMachine/HierarchicalState.lua", "numInputs", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/HierarchicalStateMachine/HierarchicalState.lua", "inputs.INPUT_ONE.value", std::uint32_t{0}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/HierarchicalStateMachine/HierarchicalState_Complex.lua", "currentState", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/HierarchicalStateMachine/HierarchicalState_Complex.lua", "numTransitions", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
    ));
/*
class HierarchicalStateMachine_testOnInput : public ::testing::TestWithParam<std::tuple<const char*, const char*>>
{
    public:
    using TestStateMachine = dagbase::HierarchicalStateMachine;
public:
    void configure(dagbase::ConfigurationElement& config)
    {
        if (auto element = config.findElement("inputs"); element)
        {
            element->eachChild([this](dagbase::ConfigurationElement& child) {
                Input input;

                input.configure(child);

                _inputs.emplace_back(input);

                return true;
            });
        }

        if (auto element=config.findElement("asserts"); element)
        {
            element->eachChild([this](dagbase::ConfigurationElement& child) {
                Assert entry;

                entry.configure(child);
                _asserts.emplace_back(entry);

                return true;
            });


        }
    }

    void makeItSo(TestStateMachine& sut)
    {
        for (auto a : _asserts)
        {
            a.makeItSo(sut);
        }
    }
protected:
    struct Input
    {
        dagbase::Atom input;
        dagbase::Atom nextState;
        bool accepted{false};

        void configure(dagbase::ConfigurationElement& config)
        {
            dagbase::ConfigurationElement::readConfig(config, "input", &input);
            dagbase::ConfigurationElement::readConfig(config, "nextState", &nextState);
            dagbase::ConfigurationElement::readConfig(config, "accepted", &accepted);
        }
    };
    using InputArray = std::vector<Input>;
    InputArray _inputs;
    struct Assert
    {
        dagbase::Atom path;
        dagbase::Variant value;

        void configure(dagbase::ConfigurationElement& config)
        {
            dagbase::ConfigurationElement::readConfig(config, "path", &path);
            dagbase::ConfigurationElement::readConfig(config, "value", &value);
        }

        void makeItSo(TestStateMachine & sut)
        {
            ASSERT_FALSE(path.empty());
            auto actualValue = sut.find(path.value());
            assertComparison(value, actualValue, 0.0, dagbase::ConfigurationElement::RELOP_EQ);// << "Expected " << value << ", got " << actualValue;
        }
    };
    using AssertArray = std::vector<Assert>;
    AssertArray _asserts;
};

TEST_P(HierarchicalStateMachine_testOnInput, testExpectedNestState)
{
    auto testConfigStr = std::get<0>(GetParam());
    dagbase::Lua testLua;
    dagbase::ConfigurationElement* testConfig = nullptr;
    {

        testConfig = dagbase::ConfigurationElement::fromFile(testLua, testConfigStr);
        ASSERT_NE(nullptr, testConfig);
    }
    configure(*testConfig);
    auto configStr = std::get<1>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    TestStateMachine sut;
    sut.configure(*config);
    for (auto& input : _inputs)
    {
        sut.onInput(input.input);
        EXPECT_EQ(input.nextState, sut.state().name);
        EXPECT_EQ(input.accepted, sut.accepted());
    }
    makeItSo(sut);
}

INSTANTIATE_TEST_SUITE_P(HierarchicalStateMachine, HierarchicalStateMachine_testOnInput, ::testing::Values(
    std::make_tuple("data/tests/HierarchicalStateMachine/onTest.lua", "data/tests/HierarchicalStateMachine/duplicateState.lua")
    ));
    */