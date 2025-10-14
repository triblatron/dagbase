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
        enum Colour : std::uint8_t
        {
            COLOUR_BLACK,
            COLOUR_RED
        };
    public:
        RedBlackTree(RedBlackTree* left, RedBlackTree* right, Colour colour);

        void setColour(Colour colour)
        {
            _asBits = (_asBits & ~1) | colour;
        }

        RedBlackTree* left()
        {
            auto retval = _asBits & ~1;
            return reinterpret_cast<RedBlackTree*>(retval);
        }

        RedBlackTree* right()
        {
            return _right;
        }
        Colour colour() const
        {
            return static_cast<Colour>(_asBits & 1);
        }
    private:
        union
        {
            struct
            {
                RedBlackTree *_left{nullptr};
                RedBlackTree *_right{nullptr};
            };
            RedBlackTree* _children[2];
            std::uintptr_t _asBits;
        };
    };
}