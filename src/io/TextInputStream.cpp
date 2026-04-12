//
// Created by Tony Horrobin on 12/04/2026.
//


#include "config/config.h"

#include "io/TextInputStream.h"
#include "util/Base64.h"
#include "io/BackingStore.h"

#include <sstream>
#include <iostream>

namespace dagbase
{
    TextInputStream::TextInputStream(BackingStore *store)
        :
    _store(store)
    {
        _istr = new std::istringstream((const char*)_store->buffer());
    }

    InputStream & TextInputStream::readBuf(value_type *value, std::size_t len)
    {
        if (_istr && value)
        {
            std::string encoded;
            (*_istr) >> encoded;
            _output.clear();
            base64decode((const std::uint8_t*)encoded.c_str(), encoded.length(), &_output);
            if (len == _output.size())
            {
                std::copy_n(_output.begin(), len, value);
            }
            else
            {
                std::cerr << "Warning team:Expected " << len << " bytes, got " << _output.size() << '\n';
            }
        }

        return *this;
    }

    InputStream & TextInputStream::read(Lua &lua, Variant *value)
    {
        if (value)
        {
            value->read(*this, lua);
        }

        return *this;
    }

    InputStream & TextInputStream::readHeader(std::string *className)
    {
        if (_istr && className)
        {
            (*_istr) >> (*className);
            std::string temp;
            // Should be " { "
            (*_istr) >> temp;
        }

        return *this;
    }

    InputStream & TextInputStream::readField(std::string *fieldName)
    {
        if (_istr && fieldName)
        {
            (*_istr) >> (*fieldName);
            char temp;
            // Should be " : "
            (*_istr) >> temp;
        }

        return *this;
    }

    InputStream & TextInputStream::readUInt8(std::uint8_t *value)
    {
        if (_istr && value)
        {
            int intValue{0};
            (*_istr) >> (intValue);
            *value = intValue;
        }

        return *this;
    }

    InputStream & TextInputStream::readInt8(std::int8_t *value)
    {
        if (_istr && value)
        {
            int intValue{0};
            (*_istr) >> (intValue);
            if (intValue >= std::numeric_limits<std::int8_t>::min() && intValue <= std::numeric_limits<int8_t>::max())
                *value = static_cast<std::int8_t>(intValue);
        }

        return *this;
    }

    InputStream & TextInputStream::readUInt16(std::uint16_t *value)
    {
        if (_istr && value)
        {
            (*_istr) >> (*value);
        }

        return *this;
    }

    InputStream & TextInputStream::readInt16(std::int16_t *value)
    {
        if (_istr && value)
        {
            (*_istr) >> (*value);
        }

        return *this;
    }

    InputStream & TextInputStream::readUInt32(std::uint32_t *value)
    {
        if (_istr && value)
        {
            (*_istr) >> (*value);
        }

        return *this;
    }

    InputStream & TextInputStream::readInt32(std::int32_t *value)
    {
        if (_istr && value)
        {
            (*_istr) >> (*value);
        }

        return *this;
    }

    InputStream & TextInputStream::readUInt64(std::uint64_t *value)
    {
        if (_istr && value)
        {
            (*_istr) >> (*value);
        }

        return *this;
    }

    InputStream & TextInputStream::readInt64(int64_t *value)
    {
        if (_istr && value)
        {
            (*_istr) >> (*value);
        }

        return *this;
    }

    InputStream & TextInputStream::readBool(bool *value)
    {
        if (_istr && value)
        {
            (*_istr) >> std::boolalpha >> *value;
        }

        return *this;
    }

    InputStream & TextInputStream::readFloat(float *value)
    {
        if (_istr && value)
        {
            (*_istr) >> (*value);
        }

        return *this;
    }

    InputStream & TextInputStream::readDouble(double *value)
    {
        if (_istr && value)
        {
            (*_istr) >> (*value);
        }

        return *this;
    }

    InputStream & TextInputStream::readString(std::string *value, bool quoted)
    {
        if (_istr && value)
        {
            if (quoted)
            {
                char q='\0';
                (*_istr) >> q;
                if (q!='\"')
                    return *this;
                char c='\0';
                (*_istr) >> c;
                while ((*_istr) && c != '\"')
                {
                    (*value) += c;
                    (*_istr) >> c;
                }
            }
            else
            {
                (*_istr) >> (*value);
            }
        }

        return *this;
    }

    InputStream & TextInputStream::readFooter()
    {
        if (_istr)
        {
            std::string temp;
            (*_istr) >> temp;
        }

        return *this;
    }
}
