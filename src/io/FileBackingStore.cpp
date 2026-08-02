//
// Created by tony on 02/08/2026.
//

#include "config/config.h"

#include "io/FileBackingStore.h"

namespace dagbase
{
    FileBackingStore::FileBackingStore()
    {
        // Do nothing.
    }

    void FileBackingStore::get(std::string &buffer, unsigned char delim)
    {
        if (_istr)
        {
            unsigned char c = '\0';

            while (c != delim)
            {
                (*_istr) >> c;
                if (c!=delim)
                    buffer += static_cast<char>(c);
            }
        }
    }

    void FileBackingStore::get(std::string &buffer)
    {
        if (_istr)
        {
            (*_istr) >> buffer;
        }
    }

    void FileBackingStore::get(unsigned char *buffer, std::size_t bufferSize)
    {
        if (_istr)
        {
            _istr->read(reinterpret_cast<std::istream::char_type*>(buffer), bufferSize);
        }
    }

    void FileBackingStore::put(const unsigned char *buffer, std::size_t bufferSize)
    {
        if (_ostr)
        {
            _ostr->write(reinterpret_cast<const std::ostream::char_type*>(buffer), bufferSize);
        }
    }

    void FileBackingStore::flush()
    {
        if (_ostr)
        {
            _ostr->flush();
        }
    }

    void FileBackingStore::open(Mode mode, const char* filename)
    {
        if ((mode & MODE_OUTPUT_BIT)!=0)
        {
            if (_ostr)
            {
                _ostr->close();
            }
            delete _ostr;
            _ostr = new std::ofstream(filename, std::ios::out|std::ios::binary);
        }
        if ((mode & MODE_INPUT_BIT)!=0)
        {
            if (_istr)
            {
                _istr->close();
            }
            delete _istr;
            _istr = new std::ifstream(filename, std::ios::in|std::ios::binary);
        }
    }
}
