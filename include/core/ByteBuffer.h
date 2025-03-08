//
// Created by tony on 05/03/24.
//

#pragma once

#include "config/DagBaseExport.h"

#include <vector>
#include <cstdint>
#include <iterator>

namespace dagbase
{
    //! Buffer of bytes, based on the Java 8 ByteBuffer API with simplifications
    class DAGBASE_API ByteBuffer
    {
    public:
        using BufferType = std::vector<std::uint8_t>;
    public:
        //! Prepare for a sequence of put()
        ByteBuffer& clear();

        //! Prepare for a sequence of get()
        ByteBuffer& flip();

        template<typename T>
        ByteBuffer& put(T value)
        {
            std::copy(reinterpret_cast<BufferType ::pointer>(&value), reinterpret_cast<BufferType::pointer>(&value)+sizeof(T), std::back_insert_iterator(_buf));
            _limit = _buf.size();

            return *this;
        }

        template<typename T>
        ByteBuffer& get(T* value)
        {
            if (value != nullptr && _position + sizeof(T) <= _buf.size())
            {
                std::copy(_buf.begin()+_position, _buf.begin()+_position+sizeof(T), reinterpret_cast<BufferType::pointer>(value));
                _position+=sizeof(T);
            }

            return *this;
        }

        ByteBuffer& put(const BufferType::value_type* buf, std::size_t len)
        {
            std::copy(buf, buf+len, std::back_insert_iterator(_buf));
            _limit = _buf.size();

            return *this;
        }

        ByteBuffer& get(BufferType::value_type* buf, std::size_t len)
        {
            if (_position + len <= _buf.size())
            {
                std::copy(_buf.begin()+_position, _buf.begin()+_position+BufferType::difference_type(len), buf);
                _position += BufferType::difference_type(len);
            }

            return *this;
        }

        BufferType::value_type const* buffer() const
        {
            return _buf.data();
        }

        BufferType::iterator begin()
        {
            return _buf.begin();
        }

        BufferType::const_iterator begin() const
        {
            return _buf.begin();
        }

        BufferType::iterator end()
        {
            return _buf.end();
        }

        BufferType::const_iterator end() const
        {
            return _buf.end();
        }

        std::size_t size() const
        {
            return _buf.size();
        }
    private:
        BufferType _buf;
        BufferType::difference_type _position{0};
        BufferType::size_type _limit{0};
    };
}