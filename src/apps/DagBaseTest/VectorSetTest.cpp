//
// Created by Tony Horrobin on 14/09/2025.
//

#include "util/VectorSet.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"
#include "test/TestUtils.h"

#include <gtest/gtest.h>

class VectorSetTestHelper
{
public:
    using IntVectorSet = dagbase::VectorSet<int> ;
    struct Insertion
    {
        int value{0};
        bool inserted{true};

        void configure(dagbase::ConfigurationElement& config)
        {
            dagbase::ConfigurationElement::readConfig(config, "value", &value);
            dagbase::ConfigurationElement::readConfig(config, "inserted", &inserted);
        }
    };

public:
    virtual ~VectorSetTestHelper() = default;

    void configure(dagbase::ConfigurationElement& config)
    {
        if (auto element=config.findElement("insertions"); element)
        {
            _insertions.reserve(element->numChildren());
            element->eachChild([this](dagbase::ConfigurationElement& child) {
                Insertion insertion;

                insertion.configure(child);
                _insertions.emplace_back(insertion);
                return true;
            });
        }
        dagbase::ConfigurationElement::readConfig(config, "size", &_expectedSize);
    }

    virtual std::pair<IntVectorSet::iterator, bool> insert(const Insertion& insert) = 0;

    virtual void makeItSo()
    {
        _sut.reserve(_insertions.size());
        for (auto insertion : _insertions)
        {
            auto p = insert(insertion);

            EXPECT_NE(_sut.end(), p.first);
            EXPECT_EQ(p.first, _sut.find(insertion.value));
            EXPECT_EQ(insertion.inserted, p.second);
        }
        EXPECT_EQ(_expectedSize, _sut.size());
    }
protected:
    std::vector<Insertion> _insertions;
    std::uint32_t _expectedSize{0};
    IntVectorSet _sut;
};

class VectorSet_testInsert : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>, public VectorSetTestHelper
{
public:
    std::pair<IntVectorSet::iterator, bool> insert(const Insertion& insertion) override
    {
        return _sut.insert(insertion.value);
    }
};

TEST_P(VectorSet_testInsert, testExpectedValue)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    configure(*config);
    makeItSo();
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actualValue = _sut.find(path);
    assertComparison(value, actualValue, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(VectorSet, VectorSet_testInsert, ::testing::Values(
        std::make_tuple("data/tests/VectorSet/NoDuplicates.lua", "size", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/VectorSet/OneDuplicate.lua", "size", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

class VectorSet_testEmplace : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>, public VectorSetTestHelper
{
public:
    std::pair<IntVectorSet::iterator, bool> insert(const Insertion& insertion) override
    {
        return _sut.emplace(insertion.value);
    }
};

TEST_P(VectorSet_testEmplace, testExpectedValue)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    configure(*config);
    makeItSo();
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actualValue = _sut.find(path);
    assertComparison(value, actualValue, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(VectorSet, VectorSet_testEmplace, ::testing::Values(
        std::make_tuple("data/tests/VectorSet/NoDuplicates.lua", "size", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/VectorSet/OneDuplicate.lua", "size", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));