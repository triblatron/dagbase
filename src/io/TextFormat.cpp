//
// Created by Tony Horrobin on 02/03/2025.
//

#include "config/config.h"

#include "io/TextFormat.h"
#include "io/BackingStore.h"
#include "util/PrettyPrinter.h"
#include "core/Class.h"
#include "io/BackingStore.h"
#include "util/DebugPrinter.h"
#include <sstream>

namespace dagbase
{
    TextFormat::TextFormat(BackingStore* store)
        :
    _store(store)
    {
        // Do nothing.
    }

    void TextFormat::setMode(Mode mode)
    {
        switch (mode)
        {
        case MODE_INPUT:
            {
                if (_store)
                {
                    std::string buffer;
                    _istr = new std::istringstream((const char*)_store->buffer());
                }
            }
            break;
        case MODE_OUTPUT:
            if (!_ostr)
            {
                _ostr = new std::ostringstream();
                _printer = new DebugPrinter();
                _printer->setStr(_ostr);
            }
            break;
        case MODE_UNKNOWN:
            break;
        }
    }

    void TextFormat::flush()
    {
        if (_store && _ostr)
        {
            std::string strValue = _ostr->str();
            _store->put(reinterpret_cast<const unsigned char*>(strValue.c_str()), strValue.size());
        }
    }

    void TextFormat::writeUInt32(std::uint32_t value)
    {
        if (_printer)
            _printer->print(value).print("\n");
    }

    void TextFormat::readUInt32(std::uint32_t* value)
    {
        if (_istr && value)
        {
            (*_istr) >> (*value);
        }
    }

    void TextFormat::writeString(std::string_view value)
    {
        if (_printer)
            _printer->print(value).print("\n");
    }

    void TextFormat::readString(std::string* value)
    {
        if (value)
            (*_istr) >> (*value);
    }

    void TextFormat::writeField(const char* fieldName)
    {
        if (_printer)
        {
            _printer->printIndent();
            _printer->print(fieldName).print(" : ");
            //(*_ostr) << fieldName << " : ";
        }
    }

    void TextFormat::readField(std::string* fieldName)
    {
        if (_istr && fieldName)
        {
            (*_istr) >> (*fieldName);
            char temp;
            // Should be " : "
            (*_istr) >> temp;
        }
    }

    void TextFormat::writeObject(Class* obj)
    {
        if (_printer)
            obj->writeToStream(*this);
    }

    void TextFormat::readObject(Class* obj)
    {
        if (_istr && obj)
            obj->readFromStream(*this);
    }

    void TextFormat::writeHeader(const char* className)
    {
        if (_printer)
        {
            _printer->println(className);
            _printer->println("{");
            _printer->indent();
        }
            //(*_ostr) << className << " { ";
    }

    void TextFormat::readHeader(std::string* className)
    {
        if (_istr && className)
        {
            (*_istr) >> (*className);
            std::string temp;
            // Should be " { "
            (*_istr) >> temp;
        }
    }

    void TextFormat::writeFooter()
    {
        if (_printer)
        {
            _printer->outdent();
            _printer->println("}");
        }
           // (*_ostr) << " } ";
    }

    void TextFormat::readFooter()
    {
        if (_istr)
        {
            std::string temp;
            (*_istr) >> temp;
        }
    }
}
