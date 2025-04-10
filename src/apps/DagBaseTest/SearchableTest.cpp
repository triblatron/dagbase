//
// Created by Tony Horrobin on 09/04/2025.
//

#include "util/SearchableMap.h"

#include <gtest/gtest.h>

#include "test/TestUtils.h"
#include "core/ConfigurationElement.h"
#include "core/LuaInterface.h"
#include "util/Searchable.h"

struct TestElement
{
    dagbase::ConfigurationElement::ValueType find(std::string_view path) const
    {
        dagbase::ConfigurationElement::ValueType retval;

        retval = dagbase::findEndpoint(path, "foo", foo);
        if (retval.has_value())
            return retval;

        return {};
    }

    std::int64_t foo{0};
};

struct TestCollection
{
    void configure(dagbase::ConfigurationElement& config)
    {
        config.eachChild([this](dagbase::ConfigurationElement& child)
        {
            std::string key = child.child(0)->asString();
            TestElement value;
            value.foo = child.child(1)->asInteger();
            lookup.m.insert(Map::value_type(key, value));

            return true;
        });
    }

    dagbase::ConfigurationElement::ValueType find(std::string_view path) const
    {
        dagbase::ConfigurationElement::ValueType retval{};

        retval = dagbase::findInternal(path, "lookup", lookup);
        if (retval.has_value())
            return retval;

        return {};
    }
    using Map = dagbase::SearchableMap<std::map<std::string, TestElement>>;
    Map lookup;
};

class SearchableMap_testFind : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::ConfigurationElement::ValueType, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(SearchableMap_testFind, testExpectedResult)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    TestCollection sut;
    if (auto element=config->findElement("map"); element)
        sut.configure(*element);

    auto actualValue = sut.find(path);
    assertComparison(value, actualValue, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(SearchableMap, SearchableMap_testFind, ::testing::Values(
    std::make_tuple("data/tests/Searchable/SearchableMap_oneElement.lua", "lookup.foo", std::int64_t(1), 0.0, dagbase::ConfigurationElement::RELOP_EQ)
    ));