//
// Created by Tony Horrobin on 13/10/2025.
//

#pragma once

#include "config/DagBaseExport.h"
#include "core/Variant.h"

#include <cstdint>
#include <vector>
#include <string_view>

namespace dagbase
{
    class ConfigurationElement;

    class DAGBASE_API RedBlackTreeNode
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
        RedBlackTreeNode();

        RedBlackTreeNode(RedBlackTreeNode* left, RedBlackTreeNode* right, Colour colour);

        void configure(ConfigurationElement& config);

        void setColour(Colour colour)
        {
            auto addr =  reinterpret_cast<std::uintptr_t>(_children[CHILD_LEFT]);
            addr = (addr & PTR_MASK) | (colour);
            _children[0] = reinterpret_cast<RedBlackTreeNode*>(addr);
        }

        Colour colour() const
        {
            auto addr = reinterpret_cast<std::uintptr_t>(_children[CHILD_LEFT]);

            return static_cast<Colour>(addr & COLOUR_RED);
        }

        RedBlackTreeNode* left()
        {
            auto retval = reinterpret_cast<std::uintptr_t>(_children[CHILD_LEFT]);
            retval &= PTR_MASK;
            return reinterpret_cast<RedBlackTreeNode*>(retval);
        }

        RedBlackTreeNode* right()
        {
            return _children[CHILD_RIGHT];
        }

        Variant find(std::string_view path) const;

        static RedBlackTreeNode NULL_NODE;
    private:
        RedBlackTreeNode* _children[2]{};
    };

    struct DAGBASE_API RedBlackTreeNodePath
    {
        using Path = std::vector<std::uint32_t>;
        Path path;

        void configure(ConfigurationElement& config);
    };

    class DAGBASE_API RedBlackTree
    {
    public:
        RedBlackTree() = default;

        void configure(ConfigurationElement& config);

        RedBlackTreeNode* traverse(const RedBlackTreeNodePath& path);

        void insert(RedBlackTreeNode* parent, RedBlackTreeNode* child);

        Variant find(std::string_view path) const;
    private:
        RedBlackTreeNode* _root{&RedBlackTreeNode::NULL_NODE};
    };
}
