//
// Created by tony on 09/03/24.
//
#include "config/config.h"

#include "io/InputStream.h"

namespace dagbase
{

    InputStream &InputStream::read(std::string *value)
    {
        size_t len = 0;
        read(&len);
        value->resize(len);
        readBuf(reinterpret_cast<value_type *>(value->data()), len);
        return *this;
    }
}
