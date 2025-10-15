//
// Created by Tony Horrobin on 13/10/2025.
//

#include "config/config.h"

#include "util/RedBlackTree.h"

namespace dagbase
{
    RedBlackTree::RedBlackTree(RedBlackTree *left, RedBlackTree *right, RedBlackTree::Colour colour)
    {
        _children[CHILD_LEFT] = left;
        _children[CHILD_RIGHT] = right;
        setColour(colour);
    }
}