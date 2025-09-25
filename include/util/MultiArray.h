//
// Created by Tony Horrobin on 25/09/2025.
//

#pragma once

#include <tuple>
#include <vector>
#include <cstdlib>

namespace dagbase
{
    template<typename... Types>
    class MultiArray
    {
    public:
        void emplace_back(const Types&... values)
        {
            std::apply([&values...](auto&... arr) {
                (arr.emplace_back(values), ...);
            }, _arrays);
        }

        void reserve(std::size_t size)
        {
            std::apply([size](auto&... arr) {
                (arr.reserve(size), ...);
            }, _arrays);
        }

        template<std::size_t N>
        auto& array()
        {
            return std::get<N>(_arrays);
        }
    private:
        std::tuple<std::vector<Types>...> _arrays;
    };
}
