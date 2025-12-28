//
// Created by Tony Horrobin on 28/12/2025.
//

#pragma once

#include <utility>
#include <cstdlib>

namespace dagbase
{
    template<typename T, std::size_t Capacity>
    class FixedVector
    {
    public:
        std::size_t capacity() const
        {
            return Capacity;
        }

        std::size_t size() const
        {
            return _size;
        }

        void push_back(T value)
        {
            emplace_back(std::move(value));
        }

        template<typename... Args>
        void emplace_back(Args&&... value)
        {
            if (_size<Capacity)
            {
                new (data()+_size++) T(std::forward<Args>(value)...);
            }
        }
    private:
        alignas(T) std::uint8_t _values[Capacity*sizeof(T)]{};
        std::size_t _size{0};
        T* data()
        {
            return reinterpret_cast<T*>(&_values[0]);
        }

        const T* data() const
        {
            return reinterpret_cast<const T*>(&_values[0]);
        }
    };
}