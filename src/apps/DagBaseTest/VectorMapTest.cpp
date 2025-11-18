//
// Created by Tony Horrobin on 27/01/2025.
//

#include "util/VectorMap.h"
#include "util/VectorMultimap.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"
#include "test/TestUtils.h"

#include <gtest/gtest.h>

class VectorMap_testInsert : public ::testing::TestWithParam<std::tuple<const char*, int, int>>
{

};

using IntVectorMap = dagbase::VectorMap<std::int64_t, std::int64_t>;
using IntVectorMultimap = dagbase::VectorMultimap<std::int64_t, std::int64_t>;

TEST_P(VectorMap_testInsert, testInsert)
{
    auto configStr = std::get<0>(GetParam());
    auto key = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    IntVectorMap sut;
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromString(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto elements = config->findElement("elements");
    ASSERT_NE(nullptr, elements);
    auto results = config->findElement("results");
    ASSERT_NE(nullptr, results);
    ASSERT_EQ(elements->numChildren(), results->numChildren());
    for (std::size_t childIndex = 0; childIndex < elements->numChildren(); ++childIndex)
    {
        auto insertion = elements->child(childIndex);
        auto result = results->child(childIndex)->child(0)->asBool();
        auto actual = sut.insert(IntVectorMap::value_type(insertion->child(0)->asInteger(), insertion->child(1)->asInteger()));
        EXPECT_EQ(result, actual.second);
        EXPECT_EQ(insertion->child(0)->asInteger(), actual.first->first);
        if (result)
        {
            EXPECT_EQ(insertion->child(1)->asInteger(), actual.first->second);
        }
    }
    auto it = sut.find(key);
    ASSERT_NE(sut.end(), it);
    EXPECT_EQ(value , it->second);
}

INSTANTIATE_TEST_SUITE_P(VectorMap, VectorMap_testInsert, ::testing::Values(
    std::make_tuple("root={ elements={ { 1, 1 } }, results={ { true } } }", 1, 1),
    std::make_tuple("root={ elements={ { 2, 2 }, { 1, 1 } }, results={ { true }, { true } } }", 2, 2),
    std::make_tuple("root={ elements={ { 1, 1 }, { 1, 2 } }, results={ { true }, { false } } }", 1, 1),
    std::make_tuple("root={ elements={ { 3, 3 }, { 1, 1 }, { 2, 2 } }, results={ { true }, { true }, { true } } }", 1, 1)
));

class VectorMapTestHelper
{
public:
    using IntVectorMap = dagbase::VectorMap<int,int>;
    struct Insertion
    {
        int key{0};
        int value{0};
        bool inserted{true};

        void configure(dagbase::ConfigurationElement& config)
        {
            dagbase::ConfigurationElement::readConfig(config, "key", &key);
            dagbase::ConfigurationElement::readConfig(config, "value", &value);
            dagbase::ConfigurationElement::readConfig(config, "inserted", &inserted);
        }
    };
public:
    virtual ~VectorMapTestHelper() = default;

    void configure(dagbase::ConfigurationElement& config)
    {
        if (auto element=config.findElement("insertions"); element)
        {
            element->eachChild([this](dagbase::ConfigurationElement& child) {
                Insertion insertion;

                insertion.configure(child);

                _insertions.emplace_back(insertion);

                return true;
            });
        }
    }

    virtual std::pair<IntVectorMap::iterator,bool> insert(const Insertion& insertion) = 0;

    void makeItSo()
    {
        for (auto& insertion : _insertions)
        {
            auto p = insert(insertion);

            EXPECT_NE(_sut.end(), p.first);
            EXPECT_EQ(insertion.inserted, p.second);
            EXPECT_EQ(p.first, _sut.find(insertion.key));
        }
    }
protected:
    IntVectorMap _sut;
private:
    std::vector<Insertion> _insertions;
};

class VectorMap_testEmplace : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>, public VectorMapTestHelper
{
public:
    std::pair<IntVectorMap::iterator, bool> insert(const Insertion& insertion)
    {
        return _sut.emplace(IntVectorMap::value_type (insertion.key, insertion.value));
    }
private:
};

TEST_P(VectorMap_testEmplace, testExpectedValue)
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

INSTANTIATE_TEST_SUITE_P(VectorMap, VectorMap_testEmplace, ::testing::Values(
        std::make_tuple("data/tests/VectorMap/NoDuplicates.lua", "size", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/VectorMap/OneDuplicate.lua", "size", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

TEST(VectorMap, testDuplicateKeysAreRejected)
{
    IntVectorMap sut;

    auto p = sut.insert(IntVectorMap::value_type(1, 1));
    ASSERT_TRUE(p.second);
    auto p2 = sut.insert(IntVectorMap::value_type(1, 2));
    ASSERT_FALSE(p2.second);
    ASSERT_EQ(p.first, p2.first);
}

class VectorMap_testFind : public ::testing::TestWithParam<std::tuple<const char*, bool>>
{

};

TEST_P(VectorMap_testFind, testFind)
{
	auto configStr = std::get<0>(GetParam());
	dagbase::Lua lua;
	auto config = dagbase::ConfigurationElement::fromString(lua, configStr);
	ASSERT_NE(nullptr, config);
	auto elementsConfig= config->findElement("elements");
	ASSERT_NE(nullptr, elementsConfig);
	IntVectorMap sut;
	elementsConfig->eachChild([&sut](auto& child)
	{
		if (child.numChildren() == 2)
			sut.insert(IntVectorMap::value_type(child.child(0)->asInteger(), child.child(1)->asInteger()));

		return true;
	});
	auto searchConfig = config->findElement("search");
	ASSERT_NE(nullptr, searchConfig);
	auto key = searchConfig->asInteger();
	auto result = std::get<1>(GetParam());
	auto it = sut.find(key);
	auto actualResult = it!=sut.end();
	EXPECT_EQ(actualResult, result);
}

INSTANTIATE_TEST_SUITE_P(VectorMap, VectorMap_testFind, ::testing::Values(
	std::make_tuple("root = { elements={ }, search=1 }", false),
	std::make_tuple("root = { elements={ {1,1}, {2,2}, {3,3} }, search=1 }", true),
	std::make_tuple("root = { elements={ {1,1}, {2,2}, {3,3} }, search=4 }", false),
	std::make_tuple("root = { elements={ {1,1}, {3,3} }, search=2 }", false)
	));

class VectorMultimap_testEqualRange : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

TEST_P(VectorMultimap_testEqualRange, testFind)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromString(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto elementsConfig = config->findElement("elements");
    ASSERT_NE(nullptr, elementsConfig);
    IntVectorMultimap sut;
    elementsConfig->eachChild([&sut](auto &child) {
        if (child.numChildren() == 2)
            sut.insert(IntVectorMap::value_type(child.child(0)->asInteger(), child.child(1)->asInteger()));

        return true;
    });
    auto searchConfig = config->findElement("search");
    ASSERT_NE(nullptr, searchConfig);
    auto key = searchConfig->asInteger();
    std::vector<typename IntVectorMultimap::value_type> results;
    if (auto resultConfig = config->findElement("results"); resultConfig)
    {
        resultConfig->eachChild([&results](dagbase::ConfigurationElement &child) {
            if (child.numChildren() == 2)
            {
                results.emplace_back(child.child(0)->asInteger(0), child.child(1)->asInteger());
            }
            return true;
        });
    }


    auto p = sut.equal_range(key);
    std::vector<typename IntVectorMultimap::value_type> actualResults;
    for (auto it = p.first; it != p.second; ++it)
    {
        actualResults.emplace_back(it->first, it->second);
    }
    EXPECT_EQ(actualResults, results);
}

INSTANTIATE_TEST_SUITE_P(VectorMultimap, VectorMultimap_testEqualRange, ::testing::Values(
	std::make_tuple("root = { elements={ }, search=1, results={} }"),
	std::make_tuple("root = { elements={ {1,1}, {2,2}, {3,3} }, search=1, results={{1,1}} }"),
	std::make_tuple("root = { elements={ {1,1}, {2,2}, {3,3} }, search=4, results={} }"),
	std::make_tuple("root = { elements={ {1,1}, {3,3} }, search=2, results={} }"),
    std::make_tuple("root = { elements={ {1,1}, {1,2} }, search=1, results={{1,1},{1,2}} }"),
    std::make_tuple("root = { elements={ {2,1}, {1,1}, {1,2}, {2,2}, {3,1}, {2,3} }, search=2, results={{2,1},{2,2},{2,3}} }"),
    std::make_tuple("root = { elements={ {2,1}, {1,1}, {1,2}, {2,2}, {3,1}, {2,3} }, search=3, results={{3,1}} }"),
    std::make_tuple("root = { elements={ {2,1}, {1,1}, {1,2}, {2,2}, {3,1}, {2,3} }, search=3, results={{3,1}} }")
	));
