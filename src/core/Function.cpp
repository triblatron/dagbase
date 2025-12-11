//
// Created by Tony Horrobin on 11/11/2025.
//

#include "config/config.h"

#include "core/Function.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"
#include "core/LuaInterface.h"

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
        // This pops the value on the top of the stack and puts it in the registry table.
        _ref = luaL_ref(_lua, LUA_REGISTRYINDEX);
        // Put the value back on the top of the stack.
        lua_rawgeti(lua, LUA_REGISTRYINDEX, _ref);
    }

    OutputStream &Function::write(OutputStream &str) const
    {
        lua_rawgeti(_lua, LUA_REGISTRYINDEX, _ref);
//        str.beginSubBuffer();
        int errod = lua_dump(_lua, &writer, (void*)&str, 0);
        if (errod != LUA_OK)
        {

        }
 //       str.endSubBuffer();
        //str.writeUInt32(0);
        lua_pop(_lua,1);

        return str;
    }

    int Function::writer(lua_State *L, const void *data, std::size_t size, void *userData)
    {
        OutputStream* str = static_cast<OutputStream*>(userData);

        if (str)
        {
            str->writeUInt32(size);
            str->writeBuf((const OutputStream::value_type*)data, size);

        }

        return 0;
    }

    struct ReaderData
    {
        InputStream::value_type* buf{nullptr};
        InputStream* str{nullptr};
    };

    Function::Function(InputStream &str, Lua& lua)
    :
    _lua(lua.get())
    {
        ReaderData readerData;
        readerData.str = &str;
        int errod = lua_load(lua.get(), &reader, (void*)&readerData, "chunk", "bt");
        if (errod!=LUA_OK)
        {
            std::cerr << "Function::ctor(InputStream):Error loading chunk " << lua_tostring(_lua, -1) << '\n';
        }
        delete [] readerData.buf;
        if (!lua_isfunction(_lua, -1))
        {
            std::cerr << "Function::ctor(InputStream):Value on top of the stack is not a function, but a " << lua_typename(_lua, lua_type(_lua,-1)) << '\n';
        }
        _ref = luaL_ref(_lua, LUA_REGISTRYINDEX);
    }

    const char* Function::reader(lua_State *L, void *userData, std::size_t *size)
    {
        auto readerData = static_cast<ReaderData*>(userData);
        auto str = readerData->str;

        if (str && size)
        {
            std::uint32_t blockSize{0};

            str->readUInt32(&blockSize);

            if (blockSize)
            {
                *size = blockSize;
                delete [] readerData->buf;
                readerData->buf = new InputStream::value_type [blockSize];
                str->readBuf(readerData->buf, *size);
                return (const char*)readerData->buf;
            }
        }

        return nullptr;
    }

    bool Function::equals(const Function &other) const
    {
        if (_lua && _lua == other._lua)
        {
            lua_rawgeti(_lua, LUA_REGISTRYINDEX, _ref);

            lua_rawgeti(_lua, LUA_REGISTRYINDEX, other._ref);

            if (lua_isfunction(_lua, -1) && lua_isfunction(_lua, -2))
            {
                return true;
//                auto retval = lua_rawequal(_lua, -1, -2);
//                lua_pop(_lua, 2);
//                return retval;
            }
        }

        return false;
    }

    void Function::operator()(int nargs, int nresults)
    {
        lua_rawgeti(_lua, LUA_REGISTRYINDEX, _ref);
        int errod = lua_pcall(_lua, nargs, nargs, 0);
        if (errod!=LUA_OK)
        {
            std::cerr << "Function::operator():Got error " << lua_tostring(_lua,-1) << '\n';
            lua_pop(_lua,1);
        }
    }

}
