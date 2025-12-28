//
// Created by Tony Horrobin on 28/12/2025.
//

#include "util/FixedVector.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"
#include "util/enums.h"

#include <gtest/gtest.h>

using TestFixedVector = dagbase::FixedVector<int, 10>;

class FixedVector_testEmplaceBack : public ::testing::TestWithParam<std::tuple<const char*>>
{
public:
    struct Emplacement
    {
        int value{0};

        void configure(dagbase::ConfigurationElement& config);

        void makeItSo(TestFixedVector& sut);
    };
public:
    void configure(dagbase::ConfigurationElement& config);

    void makeItSo(TestFixedVector& sut);
private:
    std::uint32_t _expectedSize{0};
    std::vector<Emplacement> _emplacements;
};

void FixedVector_testEmplaceBack::Emplacement::configure(dagbase::ConfigurationElement &config)
{
    dagbase::ConfigurationElement::readConfig(config, "value", &value);
}

void FixedVector_testEmplaceBack::Emplacement::makeItSo(TestFixedVector &sut)
{
    sut.emplace_back(std::forward<int>(value));
}

void FixedVector_testEmplaceBack::configure(dagbase::ConfigurationElement& config)
{
    dagbase::ConfigurationElement::readConfig(config, "size", &_expectedSize);
    if (auto element=config.findElement("emplacements"); element)
    {
        element->eachChild([this](dagbase::ConfigurationElement& child) {
            Emplacement entry;

            entry.configure(child);
            _emplacements.emplace_back(entry);

            return true;
        });
    }
}

void FixedVector_testEmplaceBack::makeItSo(TestFixedVector& sut)
{
    for (auto& entry : _emplacements)
    {
        entry.makeItSo(sut);
    }
    ASSERT_EQ(_expectedSize, sut.size());
}

TEST_P(FixedVector_testEmplaceBack, testExpectedSize)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    configure(*config);
    dagbase::FixedVector<int, 10> sut;
    makeItSo(sut);;
}

INSTANTIATE_TEST_SUITE_P(FixedVector, FixedVector_testEmplaceBack, ::testing::Values(
    std::make_tuple("data/tests/FixedVector/EmplaceOne.lua"),
    std::make_tuple("data/tests/FixedVector/EmplaceMany.lua"),
    std::make_tuple("data/tests/FixedVector/EmplaceMax.lua"),
    std::make_tuple("data/tests/FixedVector/EmplaceTooMany.lua")
    ));

struct TestFixedVectorElement
{
    int x{0};
    int y{0};

    void configure(dagbase::ConfigurationElement& config);

    bool operator==(const TestFixedVectorElement& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
};

using TestFixedVectorOfStruct = dagbase::FixedVector<TestFixedVectorElement,10>;

class FixedVector_testOp : public ::testing::TestWithParam<std::tuple<const char*>>
{
public:
    struct Op
    {
        enum Cmd : std::uint32_t
        {
            CMD_UNKNOWN,
            CMD_EMPLACE_BACK,
            CMD_POP_BACK,
            CMD_EMPTY,
            CMD_SUBSCRIPT
        };

        Cmd cmd{CMD_UNKNOWN};
        TestFixedVectorElement value{};
        std::uint32_t index{0};
        std::uint32_t size{0};
        bool empty{true};
        void configure(dagbase::ConfigurationElement& config);
        void makeItSo(TestFixedVectorOfStruct& sut);
        static Cmd parseCommand(const char* str);
    };

    void configure(dagbase::ConfigurationElement& config);

    void makeItSo(TestFixedVectorOfStruct& sut);
private:
    std::vector<Op> _ops;
};

void TestFixedVectorElement::configure(dagbase::ConfigurationElement &config)
{
    dagbase::ConfigurationElement::readConfig(config, "x", &x);
    dagbase::ConfigurationElement::readConfig(config, "y", &y);
}

void FixedVector_testOp::Op::configure(dagbase::ConfigurationElement &config)
{
    dagbase::ConfigurationElement::readConfig<Cmd>(config, "cmd", &parseCommand, &cmd);
    switch (cmd)
    {
        case CMD_EMPLACE_BACK:
            dagbase::ConfigurationElement::readConfig(config, "value", &value);
            dagbase::ConfigurationElement::readConfig(config, "size", &size);
            break;
        case CMD_POP_BACK:
            dagbase::ConfigurationElement::readConfig(config, "size", &size);
            break;
        case CMD_EMPTY:
            dagbase::ConfigurationElement::readConfig(config, "empty", &empty);
            break;
        case CMD_SUBSCRIPT:
            dagbase::ConfigurationElement::readConfig(config, "index", &index);
            dagbase::ConfigurationElement::readConfig(config, "value", &value);
            break;
        default:
            break;
    }
}

void FixedVector_testOp::Op::makeItSo(TestFixedVectorOfStruct &sut)
{
    switch (cmd)
    {
        case CMD_EMPLACE_BACK:
            sut.emplace_back(value);
            EXPECT_EQ(size, sut.size());
            break;
        case CMD_POP_BACK:
            sut.pop_back();
            EXPECT_EQ(size, sut.size());
            break;
        case CMD_EMPTY:
            EXPECT_EQ(empty, sut.empty());
            break;
        case CMD_SUBSCRIPT:
            EXPECT_EQ(value, sut[index]);
            break;
        default:
            break;
    }
}

FixedVector_testOp::Op::Cmd FixedVector_testOp::Op::parseCommand(const char *str)
{
    TEST_ENUM(CMD_EMPLACE_BACK, str)
    TEST_ENUM(CMD_POP_BACK, str)
    TEST_ENUM(CMD_EMPTY, str)
    TEST_ENUM(CMD_SUBSCRIPT, str)

    return CMD_UNKNOWN;
}

void FixedVector_testOp::configure(dagbase::ConfigurationElement &config)
{
    dagbase::ConfigurationElement::readConfigVector(config, "ops", &_ops);
}

void FixedVector_testOp::makeItSo(TestFixedVectorOfStruct &sut)
{
    for (auto op : _ops)
    {
        op.makeItSo(sut);
    }
}

TEST_P(FixedVector_testOp, testExpectedState)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    configure(*config);
    TestFixedVectorOfStruct sut;
    makeItSo(sut);
}

INSTANTIATE_TEST_SUITE_P(FixedVector, FixedVector_testOp, ::testing::Values(
    std::make_tuple("data/tests/FixedVector/PushThenPop.lua"),
    std::make_tuple("data/tests/FixedVector/Subscript.lua"),
    std::make_tuple("data/tests/FixedVector/Underflow.lua")
    ));