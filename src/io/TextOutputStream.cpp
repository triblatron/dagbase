//
// Created by Tony Horrobin on 12/04/2026.
//

#include "config/config.h"

#include "io/TextOutputStream.h"
#include "util/DebugPrinter.h"
#include "util/Base64.h"
#include "io/BackingStore.h"

#include <sstream>

namespace dagbase
{
    TextOutputStream::TextOutputStream(BackingStore* store)
        :
    _store(store)
    {
        _ostr = new std::ostringstream();
        _printer = new DebugPrinter();
        _printer->setStr(_ostr);
    }

    OutputStream& TextOutputStream::beginSubBuffer()
    {
        _inSubBuffer = true;

        return *this;
    }

    OutputStream & TextOutputStream::endSubBuffer()
    {
        _inSubBuffer = false;

        return *this;
    }

    OutputStream & TextOutputStream::writeBuf(const value_type *buf, std::size_t len)
    {
        if (_printer && buf)
        {
            _output.clear();
            base64encode(buf, len, &_output);
            _printer->println(_output);
        }

        return *this;
    }

    OutputStream & TextOutputStream::writeUInt8(std::uint8_t value)
    {
        if (_printer)
            _printer->print(std::uint32_t(value)).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeInt8(std::int8_t value)
    {
        if (_printer)
            _printer->print(std::int32_t(value)).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeUInt16(std::uint16_t value)
    {
        if (_printer)
            _printer->print(value).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeInt16(std::int16_t value)
    {
        if (_printer)
            _printer->print(value).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeUInt32(std::uint32_t value)
    {
        if (_printer)
            _printer->print(value).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeInt32(std::int32_t value)
    {
        if (_printer)
            _printer->print(value).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeUInt64(std::uint64_t value)
    {
        if (_printer)
            _printer->print(value).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeInt64(std::int64_t value)
    {
        if (_printer)
            _printer->print(value).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeBool(bool value)
    {
        if (_printer)
            _printer->print(value).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeFloat(float value)
    {
        if (_printer)
            _printer->print(value).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeDouble(double value)
    {
        if (_printer)
            _printer->print(value).print("\n");

        return *this;
    }

    OutputStream & TextOutputStream::writeString(const std::string &value, bool quoted)
    {
        if (_printer)
        {
            if (quoted)
            {
                _printer->print('\"').print(value).print("\"\n");
            }
            else
            {
                _printer->print(value).print("\n");
            }
        }

        return *this;
    }

    OutputStream & TextOutputStream::writeHeader(std::string_view className)
    {
        if (_printer)
        {
            _printer->println(className);
            _printer->println("{");
            _printer->indent();
        }

        return *this;
    }

    OutputStream & TextOutputStream::writeFooter()
    {
        if (_printer)
        {
            _printer->outdent();
            _printer->println("}");
        }

        return *this;
    }

    OutputStream & TextOutputStream::writeField(std::string_view fieldName)
    {
        if (_printer)
        {
            _printer->printIndent();
            _printer->print(fieldName).print(" : ");
        }

        return *this;
    }

    OutputStream & TextOutputStream::flush()
    {
        if (_ostr)
        {
            std::string strValue = _ostr->str();
            _store->put(reinterpret_cast<const unsigned char*>(strValue.c_str()), strValue.size());
        }

        return *this;
    }
}
