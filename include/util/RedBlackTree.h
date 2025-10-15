//
// Created by Tony Horrobin on 13/10/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include <cstdint>

namespace dagbase
{
    class DAGBASE_API RedBlackTree
    {
    public:
        enum Child : std::uint32_t
        {
            CHILD_LEFT,
            CHILD_RIGHT
        };

        enum Colour : std::uint8_t
        {
            COLOUR_BLACK,
            COLOUR_RED
        };

        static constexpr std::uintptr_t PTR_MASK = ~static_cast<std::uintptr_t>(COLOUR_RED);
    public:
        RedBlackTree(RedBlackTree* left, RedBlackTree* right, Colour colour);

        void setColour(Colour colour)
        {
            auto addr =  reinterpret_cast<std::uintptr_t>(_children[CHILD_LEFT]);
            addr = (addr & PTR_MASK) | (colour);
            _children[0] = reinterpret_cast<RedBlackTree*>(addr);
        }

        RedBlackTree* left()
        {
            auto retval = reinterpret_cast<std::uintptr_t>(_children[CHILD_LEFT]);
            retval &= PTR_MASK;
            return reinterpret_cast<RedBlackTree*>(retval);
        }

        RedBlackTree* right()
        {
            return _children[CHILD_RIGHT];
        }

        Colour colour() const
        {
            auto addr = reinterpret_cast<std::uintptr_t>(_children[CHILD_LEFT]);

            return static_cast<Colour>(addr & COLOUR_RED);
        }
    private:
        RedBlackTree* _children[2];
    };
}