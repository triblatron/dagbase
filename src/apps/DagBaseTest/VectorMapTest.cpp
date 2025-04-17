//
// Created by Tony Horrobin on 27/01/2025.
//

#include "util/VectorMap.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"

#include <gtest/gtest.h>

class VectorMap_testInsert : public ::testing::TestWithParam<std::tuple<const char*, int, int>>
{

};

using IntVectorMap = dagbase::VectorMap<std::int64_t, std::int64_t>;

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
