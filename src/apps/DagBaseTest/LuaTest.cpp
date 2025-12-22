#include "config/config.h"

#include "core/LuaInterface.h"
#include "core/Function.h"

#include <gtest/gtest.h>
#include <memory>

#include "core/ConfigurationElement.h"

TEST(Coroutine, testReturnsNullWhenNotGivenAFunction)
{
	dagbase::Lua lua;
	dagbase::BalancedStackGuard guard(lua.get());
	dagbase::Coroutine* sut = dagbase::Coroutine::fromFunction(lua.get());
	EXPECT_EQ(nullptr, sut);
}

TEST(Coroutine, testReturnsACoroutineIfGivenAFunction)
{
	dagbase::Lua lua;
	dagbase::BalancedStackGuard guard(lua.get());

	lua.eval("return function() end");
	std::unique_ptr<dagbase::Coroutine> sut(dagbase::Coroutine::fromFunction(lua.get()));
	ASSERT_NE(nullptr, sut);
	EXPECT_TRUE(sut->ok());
}

class Function_testCall : public ::testing::TestWithParam<std::tuple<const char*, int, int, bool, int, dagbase::Variant>>
{

};

TEST_P(Function_testCall, testExpectedResult)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::Function* sut = nullptr;
    dagbase::ConfigurationElement::readConfig(*config, "sut", &sut);
    ASSERT_NE(nullptr, sut);
    auto nargs = std::get<1>(GetParam());
    auto nresults = std::get<2>(GetParam());
    (*sut)(nargs,nresults);
    auto ok = std::get<3>(GetParam());
    ASSERT_EQ(ok, sut->ok());
    auto index = std::get<4>(GetParam());
    auto actualResult = sut->result(index);
    auto result = std::get<5>(GetParam());
    EXPECT_EQ(result, actualResult);
    sut->cleanupResults();
}

INSTANTIATE_TEST_SUITE_P(Function, Function_testCall, ::testing::Values(
    std::make_tuple("data/tests/Lua/BadFunction.lua", 0, 0, false, 1, dagbase::Variant()),
    std::make_tuple("data/tests/Lua/OutOfRangeResult.lua", 0, 0, true, 1, dagbase::Variant()),
    std::make_tuple("data/tests/Lua/OneResult.lua", 0, 1, true, 1, 1LL),
    std::make_tuple("data/tests/Lua/MultipleResults.lua", 0, 2, true, 1, 1.5),
    std::make_tuple("data/tests/Lua/MultipleResults.lua", 0, 2, true, 2, std::string("test"))
    ));