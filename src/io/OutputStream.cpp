//
// Created by tony on 07/03/24.
//
#include "config/config.h"

#include "io/OutputStream.h"

namespace dagbase
{
    OutputStream &OutputStream::write(const std::string & value)
    {
        write(value.length());
        writeBuf(reinterpret_cast<const value_type *>(value.data()), value.length());

        return *this;
    }
}
