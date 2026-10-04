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
    class NodeLibrary;
    class OutputStream;

    //! Base class for classes that have fields and operations.
    class DAGBASE_API Class
    {
    public:
        enum Error : std::uint32_t
        {
            ERROR_NONE,
            ERROR_TYPE_NOT_FOUND,
            WARNING_MISMATCHED_IO
        };

        enum Severity : std::uint32_t
        {
            SEVERITY_NONE,
            SEVERITY_WARNING,
            SEVERITY_ERROR
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

        Error error() const
        {
            return _errod;
        }

        Severity severity() const
        {
            return _severity;
        }

        std::ostringstream & raiseError(Error code);

        std::string errorMessage() const;

        virtual OutputStream& writeToStream(OutputStream& str, NodeLibrary& nodeLib, Lua& lua) const;

        virtual InputStream& readFromStream(InputStream& str, NodeLibrary& nodeLib, Lua& lua);

        virtual OutputStream& writeFlat(OutputStream& str, NodeLibrary& nodeLib, Lua& lua) const;

        virtual InputStream& readFlat(InputStream& str, NodeLibrary& nodeLib, Lua& lua);

        static const char* errorToString(Error value);

        static Error parseError(const char* str);

        static const char* severityToString(Severity value);

        static Severity parseSeverity(const char* str);
    private:
        MetaClass* _metaClass{nullptr};
        std::ostringstream* _errorStr{ nullptr };
        Error _errod{ERROR_NONE};
        Severity _severity{SEVERITY_NONE};
    };
}
