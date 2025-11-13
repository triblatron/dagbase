//
// Created by Tony Horrobin on 11/11/2025.
//

#include "config/config.h"

#include "core/Function.h"

namespace dagbase
{
    Function::Function(lua_State *lua, bool balance)
            :
            _lua(lua),
            _balance(balance)
    {
        if (!lua_isfunction(_lua, -1))
        {
            throw std::runtime_error("Function::ctor():Expected a function");
        }
        _ref = luaL_ref(_lua, LUA_REGISTRYINDEX);
    }

}