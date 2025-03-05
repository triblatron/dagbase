#include "config/config.h"

#include "core/Class.h"

#include <sstream>

namespace dagbase
{
    Class::~Class()
    {
        delete _errorStr;
    }

    std::ostringstream & Class::raiseError( int code )
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

    void Class::writeToStream(BackingStore& store, StreamFormat& format) const
    {
        // str << "Class { errod: " << _errod << " }";
        format.writeHeader(store, "Class");
        format.writeField(store, "errod");
        format.writeUInt32(store, _errod);
        format.writeFooter(store);
    }

    void Class::readFromStream(BackingStore& store, StreamFormat& format)
    {

        std::string className;
        format.readHeader(store, &className);

        if (className!="Class")
            return;
        // char temp;
        // str >> temp;
        // if (temp!='{')
        //     return;
        std::string fieldName;
        format.readField(store, &fieldName);
        // str >> fieldName;
        if (fieldName!="errod")
            return;
        std::uint32_t errod;
        // str >> errod;
        format.readUInt32(store, &errod);

        _errod = Error(errod);
        // str >> temp;
        // if (temp!='}')
        //     return;
        format.readFooter(store);
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
