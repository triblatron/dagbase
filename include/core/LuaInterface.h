#pragma once

#include "config/DagBaseExport.h"

#include "core/Function.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "util/errod.h"

#include <filesystem>

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <sstream>
#include <functional>

namespace dagbase
{
    class DAGBASE_API BalancedStackGuard
    {
    public:
        explicit BalancedStackGuard(lua_State* lua);

        ~BalancedStackGuard();
    private:
        lua_State* _lua{ nullptr };
        int _oldTop{ 0 };
    };

    class DAGBASE_API TableTraversal
    {
    public:
        explicit TableTraversal( lua_State * lua )
            :
        _lua(lua)
        {
            assert( lua_istable( _lua, -1 ) );

            _oldTop = lua_gettop(_lua);
        }

        //! We want a very loud failure if the stack is not balanced
        //! at this point.
        ~TableTraversal()
        {
            assert( _oldTop == lua_gettop(_lua) );
        }

        //! Traverse the table, calling the callable f on every value found
        template<typename F>
        int operator()(F f, size_t level=0)
        {
            // Dummy key to prepare for first call to lua_next().
            lua_pushnil(_lua);

            while (lua_next(_lua, -2) != 0)
            {
                int resultCode = std::invoke(f, _lua, level);
                if (resultCode != 0)
                {
                    // Pop the key and the value.
                    lua_pop(_lua, 2);
                    return resultCode;
                }
                if (lua_istable(_lua, -1))
                {
                    resultCode = (*this)(f, level+1);
                }
                // Pop the value.
                lua_pop(_lua, 1);
                if (resultCode != 0)
                {
                    // Pop the remaining key.
                    lua_pop(_lua, 1);
                    return resultCode;
                }
            }

            return 0;
        }
    private:
        lua_State * _lua;
        int _oldTop;
    };

    class DAGBASE_API Table
    {
    public:
        Table(lua_State *L,const char *name, bool cleanup = true)
            :
            _lua(L),
            _cleanup(cleanup)
        {
            lua_pushstring(L, name);
            lua_rawget(L, -2);
            if (!lua_istable(L, -1))
            {
                // Restore balance.
                lua_pop(L, 1);
                lua_newtable(L);
            }
        }

        Table(lua_State *L, int index, bool cleanup = true)
            :
            _lua(L),
            _cleanup(cleanup)
        {
            lua_pushinteger(L, index);
            lua_rawget(L, -2);
            if (!lua_istable(L, -1))
            {
                // Restore balance.
                lua_pop(L, 1);
                lua_newtable(L);
            }
        }

        explicit Table(lua_State *L, bool cleanup)
            :
            _lua(L),
            _cleanup(cleanup)
        {
            assert( _lua != nullptr );
            assert( lua_istable( _lua, -1 ) );
        }

        static Table global(lua_State* L, const char* name)
        {
            lua_getglobal(L, name);
            if (!lua_istable(L, -1))
            {
                lua_pop(L,1);
                lua_newtable(L);
                lua_setglobal(L, name);
                lua_getglobal(L,name);
            }

            return Table(L, true);
        }

        ~Table()
        {
            // Restore balance to the stack.
            // We do not own the lua_State instance.
            if (_cleanup)
            {
                lua_pop(_lua, 1);
            }
        }

        lua_State * get()
        {
            return _lua;
        }

        /**
         *
         * @return The length of this Table, equivalent to #t in Lua.
         */
        lua_Integer length() const
        {
            lua_len(_lua, -1);
            lua_Integer len = lua_tointeger(_lua, -1);
            lua_pop(_lua, 1);

            return len;
        }

        bool isTable(const char * name) const
        {
            lua_pushstring(_lua, name);
            lua_rawget(_lua, -2);
            bool retval = lua_istable(_lua, -1);

            lua_pop(_lua, 1);

            return retval;
        }

        Table tableForName(const char * name)
        {
            return {_lua,name};
        }

        Table tableForIndex(int index)
        {
            return {_lua, index};
        }

        double numberForIndexOrDefault(int i, double defaultValue=0.0)
        {
            lua_pushinteger(_lua, i);
            lua_rawget(_lua, -2);
            if (lua_isnumber(_lua, -1))
            {
                double value = lua_tonumber(_lua, -1);
                lua_pop(_lua, 1);

                return value;
            }
            else
            {
                lua_pop(_lua, 1);
                return defaultValue;
            }
        }

        double numberForNameOrDefault(const char *key, double defaultValue=0.0)
        {
            lua_pushstring(_lua, key);
            lua_rawget(_lua, -2);
            if (lua_isnumber(_lua, -1))
            {
                double value = lua_tonumber(_lua, -1);
                lua_pop(_lua, 1);

                return value;
            }
            else
            {
                lua_pop(_lua, 1);

                return defaultValue;
            }
        }

        lua_Integer integerForIndexOrDefault(int i, lua_Integer defaultValue)
        {
            lua_pushinteger(_lua, i);
            lua_rawget(_lua, -2);
            if (lua_isinteger(_lua, -1))
            {
                lua_Integer val = lua_tointeger(_lua, -1);
                lua_pop(_lua, 1);
                return val;
            }
            else
            {
                lua_pop(_lua, 1);
                return defaultValue;
            }
        }

        lua_Integer integerForNameOrDefault(const char * key, lua_Integer defaultValue=0)
        {
            lua_pushstring(_lua, key);
            lua_rawget(_lua, -2);
            if ( lua_isinteger(_lua, -1))
            {
                lua_Integer val = lua_tointeger(_lua, -1);

                lua_pop(_lua, 1);
                return val;
            }
            else
            {
                lua_pop(_lua, 1);

                return defaultValue;
            }
        }

        const char * stringForIndexOrDefault(int i, const char * defaultValue="")
        {
            lua_pushinteger(_lua, i);
            lua_rawget(_lua, -2);
            if (lua_isstring(_lua, -1))
            {
                const char * value = lua_tostring(_lua, -1);
                lua_pop(_lua, 1);

                return value;
            }
            else
            {
                lua_pop(_lua, 1);

                return defaultValue;
            }
        }

        const char * stringForNameOrDefault(const char * key, const char * defaultValue="")
        {
            lua_pushstring(_lua, key);
            lua_rawget(_lua, -2);
            if (lua_isstring(_lua, -1))
            {
                const char * value = lua_tostring(_lua, -1);
                lua_pop(_lua, 1);

                return value;
            }
            else
            {
                lua_pop(_lua, 1);

                return defaultValue;
            }
        }

        bool booleanForIndexOrDefault(int i, bool defaultValue=false)
        {
            lua_pushinteger(_lua, i);
            lua_rawget(_lua, -2);

            if ( lua_isboolean(_lua,-1) )
            {
                bool b = lua_toboolean(_lua, -1);

                lua_pop(_lua, 1);

                return b;
            }
            else
            {
                lua_pop(_lua,1);

                return defaultValue;
            }
        }

        bool booleanForNameOrDefault(const char * key, bool defaultValue=false)
        {
            lua_pushstring(_lua, key);
            lua_rawget(_lua, -2);

            if ( lua_isboolean(_lua,-1) )
            {
                bool b = lua_toboolean(_lua, -1);

                lua_pop(_lua, 1);

                return b;
            }
            else
            {
                lua_pop(_lua,1);

                return defaultValue;
            }
        }

        bool functionExists(const char * key)
        {
            lua_pushstring(_lua, key);
            lua_rawget(_lua, -2);
            bool retVal = lua_isfunction(_lua, -1);
            lua_pop(_lua, 1);

            return retVal;
        }

        //! Put a named function on the top of the stack.
        Function function(const char * key)
        {
            return {_lua, key};
        }
    protected:
        explicit Table(lua_State *L)
            :
            _lua(L),
            _cleanup(true)
        {
            assert( _lua != nullptr );
        }
    private:
        lua_State * _lua;
        bool _cleanup;
    };

    class DAGBASE_API Lua
    {
    public:
        enum Error
        {
            NoError,
            ScriptNotFound,
            FailedToExecute,
            InvalidArgument,
            NumErrors
        };
    public:
        Lua()
            :
            _own(true),
            _errod(&_errors[NoError])
        {
            _thread = luaL_newstate();
            luaL_openlibs(_thread);
        }

        explicit Lua(lua_State *L, bool own)
            :
            _thread(L),
            _own(own),
            _errod(&_errors[NoError])
        {
            // Do nothing.
        }

        ~Lua()
        {
            if (_own && _thread != nullptr)
            {
                lua_close(_thread);
            }
        }

        lua_State * get()
        {
            return _thread;
        }

        void eval(const char* code)
        {
            if (code != nullptr)
            {
                int errod = luaL_dostring(_thread,code);

                if (errod != 0)
                {
                    std::ostringstream& str = raiseError(FailedToExecute);
                    str << '\"' << code << "\":" << lua_tostring(_thread, -1);

                    std::cerr << str.str() << '\n';
                }
            }
        }

        void execute(const char * filename)
        {
            if (!std::filesystem::exists(std::string(filename)))
            {
                raiseError(ScriptNotFound) << '\"' << filename << '\"';

                return;
            }

            int errod = luaL_dofile(_thread, filename);

            if (errod != 0)
            {
                auto& str = raiseError(FailedToExecute);
                str <<  '\"' << filename << "\":" << lua_tostring(_thread,-1);
                std::cerr << str.str() << '\n';
            }
        }

        bool ok() const
        {
            return _errod == &_errors[NoError];
        }

        std::ostringstream & raiseError(Error code)
        {
            _errod = &_errors[code];
            _errorStr << _errod->name << ":description:" << _errod->description;

            return _errorStr;
        }

        std::string errorMessage() const
        {
            return _errorStr.str();
        }

        bool tableExists(const char* name) const
        {
            lua_getglobal(_thread, name);
            bool exists = lua_istable(_thread,1);
            lua_pop(_thread,1);
            return exists;
        }
        
        Table tableForName(const char* name)
        {
            return Table::global(_thread,name);
        }

        //! Get an integer at the specified 1-based index.
        int                         integer(int index)
        {
            return lua_tointeger(_thread, index);
        }

        //! Get an number at the specified 1-based index.
        double                      number(int index)
        {
            return lua_tonumber(_thread, index);
        }

        //! Get an user data at the specified 1-based index.
        void* userData(int index)
        {
            return lua_touserdata(_thread, index);
        }
    protected:
        lua_State* _thread{ nullptr };
        bool _own{ false };
    private:
        const ErrorDescriptor * _errod;
        std::ostringstream _errorStr;
        static const ErrorDescriptor _errors[NumErrors+1];
    };

    class DAGBASE_API Coroutine : public Lua
    {
    public:
        //! @constructor From a parent thread.
        //! @param[in] lua The parent of the new thread.
        //! @param[in] function The name of the function to be run in the new thread.
        Coroutine(lua_State* lua, const char* function);
        ~Coroutine();

        int                         resume(int numArgs = 0, int* numResults=nullptr);

        //! Named constructor for an existing thread.
        static Coroutine*           fromExistingThread(lua_State* existingThread);
        //! Named constructor if there is a function on the top of the stack
        static Coroutine*           fromFunction(lua_State* lua);
        static Coroutine*           fromNamedFunction(lua_State* lua, const char* function);

        int func() const
        {
            return _funcRef;
        }
    private:
        explicit Coroutine(lua_State* existingThread, bool own=false);
        Coroutine(lua_State* existingThread, int ref, bool own=false);
    public:
        int _funcRef{ LUA_NOREF };
    };

    extern void DAGBASE_API printStack(lua_State* L);
}
