#include "config/config.h"

#include "core/LuaInterface.h"
#include "util/dbc.h"
#include "util/ErrorHandler.h"

namespace dagbase
{
    void
        printStack(lua_State* L)
    {
        for (int i = 1; i <= lua_gettop(L); ++i)
        {
            std::cerr << i << ":type=" << lua_typename(L, lua_type(L, i));
            switch (lua_type(L, i))
            {
            case LUA_TNIL:
                std::cerr << "(nil)";
                break;
            case LUA_TNUMBER:
                std::cerr << lua_tonumber(L, i);

                break;
            case LUA_TBOOLEAN:
                std::cerr << lua_toboolean(L, i);

                break;
            case LUA_TSTRING:
                std::cerr << lua_tostring(L, i);

                break;
            case LUA_TTABLE:
                break;
            case LUA_TFUNCTION:
                break;
            case LUA_TUSERDATA:
                break;
            case LUA_TTHREAD:
                break;
            case LUA_TLIGHTUSERDATA:
                std::cerr << lua_touserdata(L, i);

                break;
            default:
                std::cerr << "unknown" << std::endl;

                break;
            }
            std::cerr << std::endl;
        }
    }

    ErrorDescriptor Lua::_errors[Lua::NumErrors+1]=
    {
        {
            NoError,
            "NoError",
            "The operation was successful"
        },
        {
            ScriptNotFound,
            "ScriptNotFound",
            "The specified script was not found:"
        },
        {
            FailedToExecute,
            "FailedToExecute",
            "The script failed to execute with error:"
        },
        {
            NumErrors,
            "UnknownError",
            "An unknown error"
        }
    };
    
    Coroutine::Coroutine(lua_State* lua, const char* function, ErrorHandler& errorHandler)
        :
        _thread(0)
    {
        SIM_REQUIRE("lua exists", lua != 0);
        SIM_REQUIRE("function name exists", function != 0);
        _thread = lua_newthread(lua);
        lua_getglobal(_thread, function);
        bool isFunction = lua_isfunction(_thread, -1);
        if (isFunction == false)
        {
            // Clean up the nil.
            lua_pop(_thread, 1);
            errorHandler.raiseError(InvalidArgument, "Could not create lua thread for '%s'", function);

            return;
        }
        SIM_ENSURE("Thread created", lua_isthread(lua, -1));
    }

    Coroutine Coroutine::fromExistingThread(lua_State* existingThread)
    {
        SIM_REQUIRE("existingThread is valid", existingThread != 0);

        return Coroutine(existingThread);
    }

    Coroutine* Coroutine::fromFunction(lua_State* lua)
    {
        if (lua != nullptr && lua_gettop(lua) >= 1 && lua_isfunction(lua,-1))
        {
            Coroutine* retval = nullptr;
            int ref = luaL_ref(lua, LUA_REGISTRYINDEX);
            lua_State* thread = lua_newthread(lua);
            lua_rawgeti(thread, LUA_REGISTRYINDEX, ref);
            if (lua_isfunction(thread, -1))
            {
                return new Coroutine(thread, ref);
            }
            else
            {
                std::cerr << "Expected function, got " << lua_typename(thread, -1) << '\n';
            }
        }
        else
        {
            std::cerr << "Coroutine::fromFunction: function: " << lua_isfunction(lua, -1) << ", top: " << lua_gettop(lua) << '\n';
            printStack(lua);
        }

        return nullptr;
    }

    Coroutine::Coroutine(lua_State* existingThread)
        :
        _thread(existingThread)
    {
        SIM_ENSURE("Valid reference to existing thread", _thread == existingThread);
    }
    
    Coroutine::Coroutine(lua_State* existingThread, int ref)
        :
        _thread(existingThread),
        _funcRef(ref)
    {
        SIM_ENSURE("Valid reference to existing thread", _thread == existingThread);
    }

    Coroutine::~Coroutine()
    {
        _thread = reinterpret_cast<lua_State*>(~0LL);
    }

    int Coroutine::resume(dagbase::ErrorHandler& errorHandler, int numArgs, int* numResults)
    {
        //      SIM_REQUIRE( "Main function at top of stack", lua_isfunction( _thread, -1 ) );
#if LUA_VERSION_NUM == 501
        int reasonCode = lua_resume(_thread, numArgs);
#elif LUA_VERSION_NUM == 503
        int reasonCode = lua_resume(_thread, NULL, numArgs);
#elif LUA_VERSION_NUM == 504
        int reasonCode = lua_resume(_thread, NULL, numArgs, numResults);
#else
#error "Unknown Lua version"
#endif

        return reasonCode;
    }
}
