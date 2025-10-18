//
// Created by Tony Horrobin on 13/10/2025.
//

#pragma once

#include "config/DagBaseExport.h"
#include "core/Variant.h"
#include "util/SearchableArray.h"

#include <array>
#include <cstdint>
#include <vector>
#include <string_view>

namespace dagbase
{
    class ConfigurationElement;

    class DAGBASE_API RedBlackTreeNode
    {
    public:
        //! Index into our child list
        enum Child : std::uint8_t
        {
            CHILD_LEFT,
            CHILD_RIGHT
        };

        enum Colour : std::uint8_t
        {
            COLOUR_BLACK,
            COLOUR_RED
        };

        //! The index into our parents child list of this node
        //! \note This is a bit.
        enum Direction : std::uint8_t
        {
            DIR_LEFT,
            DIR_RIGHT=1<<1
        };

        static constexpr std::uintptr_t PTR_MASK = ~(static_cast<std::uintptr_t>(COLOUR_RED)|static_cast<uintptr_t>(DIR_RIGHT));
        static constexpr std::uintptr_t COLOUR_MASK = 1<<0;
        static constexpr std::uintptr_t DIR_MASK = 1<<1;
    public:
        RedBlackTreeNode();

        RedBlackTreeNode(RedBlackTreeNode* left, RedBlackTreeNode* right, Colour colour, Direction dir);

        void configure(ConfigurationElement& config);

        void setDirection(Direction dir)
        {
            auto addr = reinterpret_cast<std::uintptr_t>(_children.a[CHILD_LEFT]);
            auto flags = (addr & ~(PTR_MASK | DIR_MASK));
            addr = (addr & PTR_MASK) | (flags | dir);
            _children.a[CHILD_LEFT] = reinterpret_cast<RedBlackTreeNode*>(addr);
        }

        Direction direction() const
        {
            auto addr = reinterpret_cast<std::uintptr_t>(_children.a[CHILD_LEFT]);

            return static_cast<Direction>(addr & DIR_RIGHT);
        }

        void setColour(Colour colour)
        {
            auto addr =  reinterpret_cast<std::uintptr_t>(_children.a[CHILD_LEFT]);
            auto flags = (addr & ~(PTR_MASK | COLOUR_MASK));
            addr = (addr & PTR_MASK) | (flags | colour);
            _children.a[0] = reinterpret_cast<RedBlackTreeNode*>(addr);
        }

        Colour colour() const
        {
            auto addr = reinterpret_cast<std::uintptr_t>(_children.a[CHILD_LEFT]);

            return static_cast<Colour>(addr & COLOUR_RED);
        }

        RedBlackTreeNode* left()
        {
            auto retval = reinterpret_cast<std::uintptr_t>(_children.a[CHILD_LEFT]);
            retval &= PTR_MASK;
            return reinterpret_cast<RedBlackTreeNode*>(retval);
        }

        RedBlackTreeNode* right()
        {
            return _children.a[CHILD_RIGHT];
        }

        bool operator==(const RedBlackTreeNode& other) const;

        Variant find(std::string_view path) const;

        std::string toString() const;

        static const char* colourToString(Colour value);

        static Colour parseColour(const char* str);

        static RedBlackTreeNode NULL_NODE;
    private:
        Variant _value;
        SearchableArray<std::array<RedBlackTreeNode*,2>> _children;
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

        bool operator==(const RedBlackTree& other) const;

        RedBlackTreeNode* traverse(const RedBlackTreeNodePath& path);

        void insert(RedBlackTreeNode* parent, RedBlackTreeNode* child);

        Variant find(std::string_view path) const;
    private:
        RedBlackTreeNode* _root{&RedBlackTreeNode::NULL_NODE};
    };
}
