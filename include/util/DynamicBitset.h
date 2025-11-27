//
// Created by Tony Horrobin on 27/11/2025.
//

#pragma once

#include <vector>
#include <limits>

namespace dagbase
{
    template<typename Block>
    class DynamicBitset
    {
    public:
        using block_type = Block;
    public:
        void push_back(bool bit)
        {
            std::size_t blockIndex = _size / (sizeof(block_type)*CHAR_BIT);
            std::size_t indexWithinBlock = _size % (sizeof(block_type)*CHAR_BIT);
            if (blockIndex >= _rep.size())
            {
                _rep.emplace_back();
            }
            _rep[blockIndex] |= bit?1<<indexWithinBlock:0;
            ++_size;
        }

        bool empty() const
        {
            return _size == 0;
        }

        std::size_t size() const
        {
            return _size;
        }

        bool testBit(std::size_t which) const
        {
            std::size_t blockIndex = which / (sizeof(block_type)*CHAR_BIT);
            std::size_t indexWithinBlock = which % (sizeof(block_type)*CHAR_BIT);

            if (blockIndex<_rep.size())
            {
                block_type block = _rep[blockIndex];
                block_type mask = 1<< indexWithinBlock;
                block_type result = (block & mask);
                return result!=0;
            }
            else
                return false;
        }
    private:
        std::vector<block_type> _rep;
        std::size_t _size{0};
    };
}
