//
// Created by Tony Horrobin on 12/04/2026.
//


#include "config/config.h"

#include "io/TextInputStream.h"
#include "util/Base64.h"
#include "io/BackingStore.h"

#include <sstream>
#include <iostream>
#include <limits>
#include <algorithm>

namespace dagbase
{
    TextInputStream::TextInputStream(BackingStore *store)
        :
    _store(store)
    {
        _istr = store->istr();
    }

    TextInputStream::~TextInputStream()
    {
        // Do nothing.
    }

    InputStream & TextInputStream::readVariableLengthInteger(std::size_t moreBit, std::uint64_t* value)
    {
        std::uint8_t moreBitMask = 1<<moreBit;
        std::uint8_t byteFromStream{0};
        readUInt8(&byteFromStream);
        auto decoded = static_cast<std::uint64_t>(byteFromStream & ~moreBitMask);
        std::uint8_t shift = moreBit;
        while ((byteFromStream & moreBitMask)!=0)
        {
            readUInt8(&byteFromStream);
            decoded |= (byteFromStream & 127u) << shift;
            shift += 7;
            moreBitMask = 1<<7;
        }
        if (value)
        {
            *value = decoded;
        }

        return *this;
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
