//
// Created by Tony Horrobin on 11/11/2025.
//

#pragma once

#include "config/DagBaseExport.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <stdexcept>

namespace dagbase
{
    class InputStream;
    class Lua;
    class OutputStream;

    class DAGBASE_API Function
    {
    public:
        //! Push the function onto the stack if it exists.
        //! \note balance defaults to false because we will typically want
        //! to call the function or store it as a reference which will pop the value.
        Function(lua_State * lua, const char * name, bool balance=false)
        :
        _lua(lua),
        _balance(balance)
        {
            lua_pushstring(_lua, name);
            lua_rawget(_lua, -2);
            if (!lua_isfunction(_lua, -1))
            {
                throw std::runtime_error(name);
            }
        }

        explicit Function(lua_State* lua, bool balance=false);

        explicit Function(InputStream& str, Lua& lua);

        //! Pop the Lua stack if balance is true.
        ~Function()
        {
            if (_balance)
            {
                lua_pop(_lua, 1);
            }
        }

        void operator()(int nargs, int nresults);

        bool equals(const Function& other) const;

        OutputStream& write(OutputStream& str) const;

        static int writer(lua_State* L, const void* data, std::size_t size, void* userData);

        static const char* reader(lua_State* L, void* userData, std::size_t* size);
    private:
        lua_State * _lua{nullptr};
        int _ref{LUA_NOREF};
        bool _balance{false};
    };
}