#include "config/config.h"

#include "core/Class.h"
#include "io/OutputStream.h"
#include "io/InputStream.h"

#include <sstream>

#include "core/NodeLibrary.h"
#include "io/OutputStream.h"

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
        case NoError:
            break;
        case TypeNotFound:
            (*_errorStr) << "TypeNotFound:";
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
