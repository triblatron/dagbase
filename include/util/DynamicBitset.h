//
// Created by Tony Horrobin on 27/11/2025.
//

#pragma once

#include <vector>
#include <limits>
#include <climits>

namespace dagbase
{
    template<typename Block>
    class DynamicBitset
    {
    public:
        using block_type = Block;
        static constexpr std::size_t bitsPerBlock = sizeof(block_type)*CHAR_BIT;
    public:
        void push_back(bool bit)
        {
            std::size_t blockIndex = _size / bitsPerBlock;
            std::size_t indexWithinBlock = _size % bitsPerBlock;
            if (blockIndex >= _rep.size())
            {
                _rep.emplace_back();
            }
            _rep[blockIndex] |= bit?1<<indexWithinBlock:0;
            ++_size;
        }

        void pop_back()
        {
            if (!empty())
            {
                --_size;
                std::size_t blockIndex = (_size) / bitsPerBlock;
                std::size_t indexWithinBlock = (_size) % bitsPerBlock;
                block_type keepMask = (1 << indexWithinBlock) - 1;

                _rep[blockIndex] &= keepMask;
            }
        }

        bool empty() const
        {
            return _size == 0;
        }

        void resize(std::size_t n, bool value=false)
        {
            std::size_t blockIndex = n / (bitsPerBlock);
            std::size_t numBitsWithinBlock = n % bitsPerBlock;

            if (n > size())
            {
                if (blockIndex>=numBlocks())
                {
                    block_type fillValue = value?std::numeric_limits<block_type>::max():0;
                    // This will fill whole blocks but might overrun the size requested.
                    _rep.resize(blockIndex+(numBitsWithinBlock!=0?1:0), fillValue);
                    // Get the mask for the number of bits within the last partial block
                    // we want to set.
                    block_type newMask = (1<<numBitsWithinBlock)-1;
                    // We only need to write if the mask has ones
                    if (value && blockIndex<numBlocks())
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
                if (oldBlockIndex < numBlocks() && oldNumBitsWithinBlock > 0)
                {
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
                }
            }
            else if (n < size())
            {
                _rep.resize(blockIndex+1);
                block_type keepMask = (1<<numBitsWithinBlock)-1;
                _rep[blockIndex] &= keepMask;
            }
            _size = n;
        }

        std::size_t size() const
        {
            return _size;
        }

        void reserve(std::size_t n)
        {
            std::size_t blockIndex = n / bitsPerBlock;
            _rep.reserve(blockIndex+1);
        }

        std::size_t capacity() const
        {
            return _rep.capacity() * bitsPerBlock;
        }

        std::size_t numBlocks() const
        {
            return _rep.size();
        }

        void setBit(std::size_t which)
        {
            std::size_t blockIndex = which / bitsPerBlock;
            std::size_t indexWithinBlock = which % bitsPerBlock;

            if (blockIndex<_rep.size())
            {
                block_type mask = 1 << indexWithinBlock;

                _rep[blockIndex] |= mask;
            }
        }

        void clearBit(std::size_t which)
        {
            std::size_t blockIndex = which / bitsPerBlock;
            std::size_t indexWithinBlock = which % bitsPerBlock;

            if (blockIndex<_rep.size())
            {
                block_type mask = 1 << indexWithinBlock;

                _rep[blockIndex] &= ~mask;
            }
        }

        void flipBit(std::size_t which)
        {
            std::size_t blockIndex = which / bitsPerBlock;
            std::size_t indexWithinBlock = which % bitsPerBlock;

            if (blockIndex<_rep.size())
            {
                block_type mask = 1 << indexWithinBlock;

                _rep[blockIndex] ^= mask;
            }
        }

        bool testBit(std::size_t which) const
        {
            std::size_t blockIndex = which / bitsPerBlock;
            std::size_t indexWithinBlock = which % bitsPerBlock;

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

        bool all() const
        {
            // Full blocks
            for (std::size_t i=0; i<numBlocks()-1; ++i)
            {
                if (_rep[i] != std::numeric_limits<block_type>::max())
                    return false;
            }

            // Partial block
            std::size_t numBitsWithinBlock = size() % bitsPerBlock;
            block_type existingMask = (1<<numBitsWithinBlock) - 1;

            if (existingMask && _rep[numBlocks()-1] != existingMask)
                return false;

            return true;
        }

        bool any() const
        {
            // Full blocks
            for (std::size_t i=0; i<numBlocks()-1; ++i)
            {
                if (_rep[i] != 0)
                    return true;
            }

            // Partial block
            std::size_t numBitsWithinBlock = size() % bitsPerBlock;
            block_type existingMask = (1<<numBitsWithinBlock) - 1;

            if (existingMask && (_rep[numBlocks()-1] & existingMask)!=0)
                return true;

            return false;
        }

        bool none() const
        {
            return !any();
        }

        std::size_t count() const
        {
            std::size_t retval = 0;

            // The naive way:Count the bits one by one
            // First the full blocks
            for (size_t i=0; i<numBlocks()-1; ++i)
            {
//                for (size_t bitIndex=0; bitIndex<bitsPerBlock; ++bitIndex)
//                {
                    retval += __builtin_popcountll(static_cast<unsigned long long>(_rep[i]) & 0xffffffff);

//                    if ((_rep[i] & (1<<bitIndex)) != 0)
//                        ++retval;
//                }
            }
            // Then the partial block at the end.
            std::size_t numBitsWithinBlock = size() % bitsPerBlock;
            block_type existingMask = (1<<numBitsWithinBlock) - 1;

            retval += __builtin_popcountll(_rep[numBlocks()-1] & existingMask);
//            for (size_t bitIndex=0; bitIndex<numBitsWithinBlock; ++bitIndex)
//            {
//                if ((_rep[numBlocks()-1] & (1<<bitIndex))!=0)
//                    ++retval;
//            }
            return retval;
        }

        void fromString(std::string_view str)
        {
            resize(str.length());
            for (std::size_t i=0; i<size(); ++i)
            {
                // We parse MSB first so we have to flip the index to get which bit to write.
                if (str[i] == '1')
                {
                    setBit(size() - 1 - i);
                }
                else
                {
                    clearBit(size() - 1 - i);
                }
            }
        }

        void toString(std::string& str) const
        {
            str.resize(size());
            // We output MSB first so we have to flip the index to get the bit to test.
            for (std::size_t i=0; i<size(); ++i)
            {
                str[size() - 1 - i] = testBit(i)?'1':'0';
            }
        }

        DynamicBitset<Block>& operator&=(const DynamicBitset<Block>& other)
        {
            if (numBlocks() == other.numBlocks())
            {
                for (std::size_t i=0; i<numBlocks(); ++i)
                {
                    _rep[i] &= other._rep[i];
                }
            }

            return *this;
        }

        DynamicBitset<Block>& operator|=(const DynamicBitset<Block>& other)
        {
            if (numBlocks() == other.numBlocks())
            {
                for (std::size_t i=0; i<numBlocks(); ++i)
                {
                    _rep[i] |= other._rep[i];
                }
            }

            return *this;
        }

        DynamicBitset<Block>& operator^=(const DynamicBitset<Block>& other)
        {
            if (numBlocks() == other.numBlocks())
            {
                for (std::size_t i=0; i<numBlocks(); ++i)
                {
                    _rep[i] ^= other._rep[i];
                }
            }

            return *this;
        }
    private:
        std::vector<block_type> _rep;
        std::size_t _size{0};
    };
}
