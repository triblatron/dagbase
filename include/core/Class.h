#pragma once

#include "config/DagBaseExport.h"

#include "core/LuaInterface.h"

#include <iosfwd>
#include <string>

#include "io/StreamFormat.h"

namespace dagbase
{
    class BackingStore;
    class StreamFormat;
}

namespace dagbase
{
    class MetaClass;
    class Lua;

    //! Base class for classes that have fields and operations.
    class DAGBASE_API Class
    {
    public:
        enum Error
        {
            NoError,
            TypeNotFound
        };
    public:
        Class() = default;
        explicit Class(dagbase::MetaClass* metaClass)
            :
        _metaClass(metaClass)
        {
            // Do nothing.
        }
        virtual ~Class();

        virtual const char* className() const
        {
            return "Class";
        }

        std::ostringstream & raiseError(int code);

        std::string errorMessage() const;

        virtual void writeToStream(StreamFormat& str) const;

        virtual void readFromStream(StreamFormat& str);
    private:
        Error _errod{Error::NoError};
        MetaClass* _metaClass{nullptr};
        std::ostringstream* _errorStr{ nullptr };
    };
}
