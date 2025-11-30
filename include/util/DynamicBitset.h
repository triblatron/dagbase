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

        void resize(std::size_t n, bool value=false)
        {
            const std::size_t bitsPerBlock = sizeof(block_type)*CHAR_BIT;
            std::size_t blockIndex = n / (bitsPerBlock);
            std::size_t numBitsWithinBlock = n % bitsPerBlock;

            if (n > size())
            {
                std::size_t oldNumBlocks = numBlocks();
                if (blockIndex+1>numBlocks())
                {
                    block_type fillValue = value?std::numeric_limits<block_type>::max():0;
                    // This will fill whole blocks but might overrun the size requested.
                    _rep.resize(blockIndex + 1, fillValue);
                    // Get the mask for the number of bits within the last partial block
                    // we want to set.
                    block_type newMask = (1<<numBitsWithinBlock)-1;
                    // We only need to write if the mask has ones
                    if (value)
                    {
                        _rep[blockIndex] = newMask;
                    }
                }
                // Fill existing partial blocks
                // We haven't updated size yet, so it has the previous value
                // and will give us the first partial block filled by the resize.
                std::size_t oldBlockIndex = size() / bitsPerBlock;
                // The number of bits within the partial block.
                std::size_t oldNumBitsWithinBlock = size() % bitsPerBlock;
                // A mask consisting of all the bits we want to leave alone.
                block_type existingMask = (1<<oldNumBitsWithinBlock)-1;
                // A mask for the rest of the block we want to fill.
                block_type restOfBlockMask = std::numeric_limits<block_type>::max();
                // Mask off the bits we want to leave alone.
                restOfBlockMask &= ~existingMask;
                // Set the rest of the block if we want to fill with true.
                // It will already be false by default.
                if (value)
                {
                    _rep[oldBlockIndex] |= restOfBlockMask;
                }
                _size = n;
            }
        }

        std::size_t size() const
        {
            return _size;
        }

        void reserve(std::size_t n)
        {
            std::size_t blockIndex = n / (sizeof(block_type)*CHAR_BIT);
            _rep.reserve(blockIndex+1);
        }

        std::size_t capacity() const
        {
            return _rep.capacity() * sizeof(block_type) * CHAR_BIT;
        }

        std::size_t numBlocks() const
        {
            return _rep.size();
        }

        void setBit(std::size_t which)
        {
            std::size_t blockIndex = which / (sizeof(block_type)*CHAR_BIT);
            std::size_t indexWithinBlock = which % (sizeof(block_type)*CHAR_BIT);

            if (blockIndex<_rep.size())
            {
                block_type block = _rep[blockIndex];
                block_type mask = 1 << indexWithinBlock;

                _rep[blockIndex] |= mask;
            }
        }

        void clearBit(std::size_t which)
        {
            std::size_t blockIndex = which / (sizeof(block_type)*CHAR_BIT);
            std::size_t indexWithinBlock = which % (sizeof(block_type)*CHAR_BIT);

            if (blockIndex<_rep.size())
            {
                block_type block = _rep[blockIndex];
                block_type mask = 1 << indexWithinBlock;

                _rep[blockIndex] &= ~mask;
            }
        }

        void flipBit(std::size_t which)
        {
            std::size_t blockIndex = which / (sizeof(block_type)*CHAR_BIT);
            std::size_t indexWithinBlock = which % (sizeof(block_type)*CHAR_BIT);

            if (blockIndex<_rep.size())
            {
                block_type block = _rep[blockIndex];
                block_type mask = 1 << indexWithinBlock;

                _rep[blockIndex] ^= mask;
            }
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
