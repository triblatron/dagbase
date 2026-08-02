//
// Created by Tony Horrobin on 02/03/2025.
//

#include "config/config.h"

#include "io/MemoryBackingStore.h"

#include <algorithm>
#include <sstream>

namespace dagbase
{
    MemoryBackingStore::MemoryBackingStore()
    {
        // auto buffer = _buffer.buffer();
        // if (_ostr)
        // {
        //     _istr = new std::istringstream();
        //     _istr->rdbuf(_ostr->rdbuf());
        // }
        // else
        //     _istr = new std::istringstream();
        _streamBuf = new std::stringbuf();
    }

    void MemoryBackingStore::open(Mode mode, const char* filename)
    {
        setMode(mode);
        if ((mode & MODE_OUTPUT_BIT) != 0)
        {
            delete _ostr;
            _ostr = new std::ostream(_streamBuf);
            _buffer.clear();
        }
        else if ((mode & MODE_INPUT_BIT) != 0)
        {
            delete _istr;
            _istr = new std::istream(_streamBuf);
            _buffer.flip();
        }
    }

    void MemoryBackingStore::get(std::string& buffer, unsigned char delim)
    {
        unsigned char c = '\0';

        while (c != delim)
        {
            _buffer.get(&c, 1);
            if (c!=delim)
                buffer += static_cast<char>(c);
        }
    }

    void MemoryBackingStore::get(std::string& buffer)
    {
        if (buffer.size()>=_buffer.size())
            std::copy_n(_buffer.begin(), _buffer.size(), buffer.data());
    }

    void MemoryBackingStore::get(unsigned char* buffer, std::size_t bufferSize)
    {
        _buffer.get(buffer, bufferSize);
    }

    void MemoryBackingStore::put(const unsigned char* buffer, std::size_t bufferSize)
    {
        _buffer.put(buffer, bufferSize);
    }

    void MemoryBackingStore::flush()
    {
        if (_ostr)
        {
            auto strValue = _streamBuf->str();
            if (!strValue.empty())
            {
                std::vector<std::uint8_t> buf(strValue.begin(), strValue.end());
                put(buf.data(), buf.size());
            }
            //_ostr->clear();
        }
    }
}
