//
// Created by Tony Horrobin on 27/11/2025.
//

#pragma once

#include <vector>
#include <limits>
#include <climits>
#include <string_view>
#include <string>
#include <cstdint>

namespace dagbase
{
    template<typename Block>
    class DynamicBitset
    {
    public:
        using block_type = Block;
        using const_reference = bool;
        class reference
        {
        public:
            reference(block_type* block, std::size_t bitWithinBlock)
            :
            _block(block),
            _bitWithinBlock(bitWithinBlock)
            {
                // Do nothing.
            }

            reference& operator=(bool value)
            {
                if (_block)
                {
                    if (value)
                    {
                        (*_block) |= (1 << _bitWithinBlock);
                    }
                    else
                    {
                        (*_block) &= ~(1 << _bitWithinBlock);
                    }
                }

                return *this;
            }
        private:
            void operator&();
            block_type* _block{nullptr};
            std::size_t _bitWithinBlock{0};
        };
        static constexpr std::size_t bitsPerBlock = sizeof(block_type)*CHAR_BIT;
        static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();
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
                    retval += popcount(static_cast<unsigned long long>(_rep[i]));
            }
            // Then the partial block at the end.
            std::size_t numBitsWithinBlock = size() % bitsPerBlock;
            block_type existingMask = (1<<numBitsWithinBlock) - 1;
            retval += popcount(_rep[numBlocks()-1] & existingMask);

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

        const_reference operator[](std::size_t index) const
        {
            if (index < size())
            {
                std::size_t blockIndex = index / bitsPerBlock;
                std::size_t bitWithinBlock = index % bitsPerBlock;

                return (_rep[blockIndex] & (1<<bitWithinBlock))!=0;
            }

            return false;
        }

        reference operator[](std::size_t index)
        {
            if (index<size())
            {
                std::size_t blockIndex = index / bitsPerBlock;
                std::size_t bitWithinBlock = index % bitsPerBlock;

                return reference(&_rep[blockIndex], bitWithinBlock);
            }
            return reference(nullptr, 0);
        }

        std::size_t findFirst() const
        {
            if (!empty())
            {
                // Full blocks
                for (std::size_t blockIndex=0; blockIndex<numBlocks()-1; ++blockIndex)
                {
                    auto block = _rep[blockIndex];
                    auto count = countTrailingZeros(block);

                    if (count < sizeof(std::uint64_t) * CHAR_BIT)
                        return blockIndex * bitsPerBlock + count;
                }
                // Partial last block
                std::size_t numBitsWithinBlock = size() % bitsPerBlock;
                block_type existingMask = (1<<numBitsWithinBlock) - 1;
                block_type existingBits = _rep[numBlocks()-1] & existingMask;
                if (auto count = countTrailingZeros(existingBits); count != sizeof(std::uint64_t) * CHAR_BIT)
                    return (numBlocks()-1) * bitsPerBlock + count;
            }

            return npos;
        }

        std::size_t findFirst(std::size_t pos)
        {
            if (pos < size())
            {
                if (pos < bitsPerBlock)
                {
                    std::size_t initialBitInBlock = pos % bitsPerBlock;

                    // Initial partial block
                    block_type initialExistingMask = (1<<(initialBitInBlock)) - 1;
                    block_type initialBlock = _rep[0] & ~initialExistingMask;
                    auto count = countTrailingZeros(initialBlock);
                    if (count >= pos && count < sizeof(std::uint64_t) * CHAR_BIT)
                    {
                        return count;
                    }
                }

                // Full blocks
                for (std::size_t blockIndex=1; blockIndex<numBlocks()-1; ++blockIndex)
                {
                    auto block = _rep[blockIndex];
                    auto count = countTrailingZeros(block);

                    if (count < sizeof(std::uint64_t) * CHAR_BIT)
                        return blockIndex * bitsPerBlock + count;
                }

                // Partial last block
                std::size_t numBitsWithinBlock = (pos) % bitsPerBlock;
                block_type existingMask = (1<<(numBitsWithinBlock+1)) - 1;
                block_type existingBits = _rep[numBlocks()-1] & existingMask;
                auto count = countTrailingZeros(existingBits);
                if ( count != sizeof(std::uint64_t) * CHAR_BIT)
                    return (numBlocks()-1) * bitsPerBlock + count;
            }

            return npos;
        }

        std::size_t findNext(std::size_t pos)
        {
            if (pos < size())
            {
                std::size_t initialBlockIndex = pos / bitsPerBlock;
                std::size_t initialBitInBlock = pos % bitsPerBlock;

                // Initial partial block
                if (pos < bitsPerBlock)
                {
                    block_type initialExistingMask = (1<<(initialBitInBlock+1)) - 1;
                    block_type initialBlock = _rep[initialBlockIndex] & ~initialExistingMask;
                    auto count = countTrailingZeros(initialBlock);
                    if (count > pos && count < sizeof(std::uint64_t) * CHAR_BIT)
                    {
                        return count;
                    }
                }

                else if (initialBlockIndex < numBlocks()-1)
                {
                    // Full blocks
                    for (std::size_t blockIndex=1; blockIndex<numBlocks()-1; ++blockIndex)
                    {
                        std::size_t numBitsWithinBlock = (pos+1) % bitsPerBlock;
                        block_type existingMask = ~((1<<numBitsWithinBlock)-1);
                        auto block = _rep[blockIndex] & existingMask;
                        auto count = countTrailingZeros(block);

                        if (count < sizeof(std::uint64_t) * CHAR_BIT)
                            return blockIndex * bitsPerBlock + count;
                    }
                }

                // Partial last block
                std::size_t numBitsWithinBlock = size() % bitsPerBlock;
                block_type firstValidBit = (size() - (pos+1)) % bitsPerBlock;
                block_type existingMask = (1<<numBitsWithinBlock) - 1;
                existingMask &= ~((1<<(numBitsWithinBlock - firstValidBit)) - 1);
                block_type existingBits = _rep[numBlocks()-1] & existingMask;

                if (auto count = countTrailingZeros(existingBits); count != sizeof(std::uint64_t) * CHAR_BIT)
                    return (numBlocks()-1) * bitsPerBlock + count;
            }

            return npos;
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
        static std::uint64_t popcount(std::uint64_t value)
        {
#if defined(__POPCNT__) || defined(__ARM_FEATURE_POPCNT)
            return __builtin_popcount(value);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
            return __popcnt64(value);
#elif defined(__has_builtin)
#if __has_builtin(__builtin_popcount)
            return __builtin_popcount(value);
#endif
#elif defined(__GNUC__)
            return __builtin_popcount(value);
#endif
            return 0;
        }

        static std::uint64_t countTrailingZeros(std::uint64_t value)
        {
#if defined(__ARM_FEATURE_CLZ)
            return __builtin_ctzll(value);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
            return _tzcnt_u64(value);
#elif defined(__has_builtin)
#if __has_builtin(__builtin_popcount)
            return __builtin_ctzll(value);
#endif
#elif defined(__GNUC__)
            return __builtin_ctzll(value);
#endif
            return 0;
        }
    };
}
