#pragma once

#include "config/DagBaseExport.h"

#include "core/LuaInterface.h"

#include <iosfwd>
#include <string>

namespace dagbase
{
    class BackingStore;
    class ClassDescription;
    class InputStream;
    class Lua;
    class MetaClass;
    class OutputStream;

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

        virtual void describe(ClassDescription& description) const;

        std::ostringstream & raiseError(Error code);

        std::string errorMessage() const;

        virtual void writeToStream(OutputStream& str) const;

        virtual void readFromStream(InputStream& str);
    private:
        MetaClass* _metaClass{nullptr};
        std::ostringstream* _errorStr{ nullptr };
        Error _errod{Error::NoError};
    };
}
