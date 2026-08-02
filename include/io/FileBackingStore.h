//
// Created by tony on 02/08/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "io/BackingStore.h"
#include "core/ByteBuffer.h"

#include <fstream>


namespace dagbase
{
    class DAGBASE_API FileBackingStore : public BackingStore
    {
    public:
        FileBackingStore();

        void open(Mode mode, const char* filename) override;

        void setStream(std::ofstream* ostr)
        {
            _ostr = ostr;
        }

        std::ostream* ostr() override
        {
            return _ostr;
        }

        std::istream* istr() override
        {
            return _istr;
        }

        void get(std::string& buffer, unsigned char delim) override;

        void get(std::string& buffer);

        void get(unsigned char* buffer, std::size_t bufferSize) override;

        void put(const unsigned char* buffer, std::size_t bufferSize) override;

        void flush() override;
    private:
        ByteBuffer _buffer;
        std::ofstream* _ostr{nullptr};
        std::ifstream* _istr{nullptr};
    };
}
