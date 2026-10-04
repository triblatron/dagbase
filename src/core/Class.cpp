#include "config/config.h"

#include "core/Class.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"
#include "core/NodeLibrary.h"
#include "util/enums.h"

#include <sstream>

namespace dagbase
{
    Class::~Class()
    {
        delete _errorStr;
    }

    void Class::describe(ClassDescription &description) const
    {
        // Do nothing.
    }

    std::ostringstream & Class::raiseError( Error code )
    {
        if (_errorStr==nullptr)
            _errorStr=new std::ostringstream();

        switch ( code )
        {
        case ERROR_NONE:
            _severity = SEVERITY_NONE;
            _errod = ERROR_NONE;
            break;
        case ERROR_TYPE_NOT_FOUND:
            _severity = SEVERITY_ERROR;
            _errod = ERROR_TYPE_NOT_FOUND;
            (*_errorStr) << "ERROR_TYPE_NOT_FOUND:";
            break;
        case WARNING_MISMATCHED_IO:
            _severity = SEVERITY_WARNING;
            _errod = WARNING_MISMATCHED_IO;
            (*_errorStr) << "WARNING_MISMATCHED_IO:";
            break;
        }

        return *_errorStr;
    }

    std::string Class::errorMessage() const
    {
        if (_errorStr!=nullptr)
            return _errorStr->str();
        else
            return {};
    }

    OutputStream& Class::writeToStream(OutputStream& format, NodeLibrary& nodeLib, Lua& lua) const
    {
        // str << "Class { errod: " << _errod << " }";
        format.writeHeader("Class");
        format.writeField("errod");
        format.writeUInt32(_errod);
        format.writeFooter();

        return format;
    }

    InputStream& Class::readFromStream(InputStream& format, NodeLibrary& nodeLib, Lua& lua)
    {

        std::string className;
        format.readHeader(&className);

        if (className!="Class")
            return format;
        // char temp;
        // str >> temp;
        // if (temp!='{')
        //     return;
        std::string fieldName;
        format.readField(&fieldName);
        // str >> fieldName;
        if (fieldName!="errod")
            return format;
        std::uint32_t errod;
        // str >> errod;
        format.readUInt32(&errod);

        _errod = Error(errod);
        // str >> temp;
        // if (temp!='}')
        //     return;
        format.readFooter();

        return format;
    }

    OutputStream & Class::writeFlat(OutputStream &str, NodeLibrary &nodeLib, Lua &lua) const
    {
        str.writeHeader("Class");
        str.writeField("errod");
        str.writeUInt32(_errod);
        str.writeFooter();

        return str;
    }

    InputStream & Class::readFlat(InputStream &str, NodeLibrary &nodeLib, Lua &lua)
    {
        std::string className;
        str.readHeader(&className);

        if (className!="Class")
            return str;
        std::string fieldName;
        str.readField(&fieldName);
        if (fieldName!="errod")
            return str;
        std::uint32_t errod;
        str.readUInt32(&errod);

        _errod = Error(errod);
        str.readFooter();

        return str;
    }

    const char * Class::errorToString(Error value)
    {
        switch (value)
        {
            ENUM_NAME(ERROR_NONE)
            ENUM_NAME(ERROR_TYPE_NOT_FOUND)
            ENUM_NAME(WARNING_MISMATCHED_IO)
        }

        return "<error>";
    }

    Class::Error Class::parseError(const char *str)
    {
        TEST_ENUM(ERROR_NONE, str)
        TEST_ENUM(ERROR_TYPE_NOT_FOUND, str)
        TEST_ENUM(WARNING_MISMATCHED_IO, str)

        return ERROR_NONE;
    }

    const char * Class::severityToString(Severity value)
    {
        switch (value)
        {
            ENUM_NAME(SEVERITY_NONE)
            ENUM_NAME(SEVERITY_WARNING)
            ENUM_NAME(SEVERITY_ERROR)
        }

        return "<error>";
    }

    Class::Severity Class::parseSeverity(const char *str)
    {
        TEST_ENUM(SEVERITY_NONE, str)
        TEST_ENUM(SEVERITY_WARNING, str)
        TEST_ENUM(SEVERITY_ERROR, str)

        return SEVERITY_NONE;
    }

    // void Class::setField( size_t index, lua_Integer value )
    // {
    //     _fields[index]->setValue(value);
    // }
    //
    // void Class::setField( size_t index, lua_Number value )
    // {
    //     _fields[index]->setValue(value);
    // }
    //
    // void Class::getField( size_t index, lua_Integer * value )
    // {
    //     *value = _fields[index]->toInteger();
    // }
    //
    // void Class::getField( size_t index, lua_Number * value )
    // {
    //     *value = _fields[index]->toNumber();
    // }
}
