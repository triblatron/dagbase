#include "config/config.h"

#include "core/LuaInterface.h"

#include <gtest/gtest.h>
#include <memory>

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
