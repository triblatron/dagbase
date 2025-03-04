#pragma once

#include "config/DagBaseExport.h"

#include "core/LuaInterface.h"

#include <iosfwd>
#include <string>

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

        std::ostringstream & raiseError(int code);

        std::string errorMessage() const;

        virtual void writeToStream(std::ostream& str) const;

        virtual void readFromStream(std::istream& str);
    private:
        MetaClass* _metaClass{nullptr};
        Error _errod{Error::NoError};
        std::ostringstream* _errorStr{ nullptr };
    };
}
