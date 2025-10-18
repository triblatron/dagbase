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

        const RedBlackTreeNode* left() const
        {
            auto retval = reinterpret_cast<std::uintptr_t>(_children.a[CHILD_LEFT]);
            retval &= PTR_MASK;
            return reinterpret_cast<const RedBlackTreeNode*>(retval);
        }

        RedBlackTreeNode* right()
        {
            return _children.a[CHILD_RIGHT];
        }

        const RedBlackTreeNode* right() const
        {
            return _children.a[CHILD_RIGHT];
        }

        RedBlackTreeNode* child(Child index)
        {
            switch (index)
            {
                case CHILD_LEFT:
                    return left();
                case CHILD_RIGHT:
                    return right();
            }
        }

        const RedBlackTreeNode* child(Child index) const
        {
            switch (index)
            {
                case CHILD_LEFT:
                    return left();
                case CHILD_RIGHT:
                    return right();
            }
        }

        bool operator==(const RedBlackTreeNode& other) const;

        Variant find(std::string_view path) const;

        std::string toString() const;

        static const char* colourToString(Colour value);

        static Colour parseColour(const char* str);

        static const char* directionToString(Direction value);

        static Direction parseDirection(const char* str);

        static Direction oppositeDirection(Direction dir)
        {
            return static_cast<Direction>(DIR_RIGHT - dir);
        }

        static const char* childToString(Child value);

        static Child parseChild(const char* str);

        static Child directionToChild(Direction dir);

        static RedBlackTreeNode NULL_NODE;
    private:
        void setChild(Child index, RedBlackTreeNode* child);
        friend class RedBlackTree;
        Variant _value;
        SearchableArray<std::array<RedBlackTreeNode*,2>> _children;
        bool compareChild(Child index, const RedBlackTreeNode& other) const;
    };

    struct DAGBASE_API RedBlackTreeNodePath
    {
        using Path = std::vector<RedBlackTreeNode::Child>;
        Path path;

        RedBlackTreeNode *ancestor(RedBlackTreeNode *root, std::size_t numGenerations)
        {
            RedBlackTreeNode* retval = root;

            if (path.size()>=numGenerations)
                for (std::size_t pathIndex = 0; pathIndex<path.size() - numGenerations; ++pathIndex)
                {
                    auto childIndex = path[pathIndex];

                    retval = retval->child(childIndex);
                }
            else
                return &RedBlackTreeNode::NULL_NODE;

            return retval;
        }

        RedBlackTreeNode* node(RedBlackTreeNode* root)
        {
            return ancestor(root, 0);
        }

        RedBlackTreeNode *parent(RedBlackTreeNode *root)
        {
            return ancestor(root, 1);
        }

        RedBlackTreeNode *grandparent(RedBlackTreeNode *root)
        {
            return ancestor(root, 2);
        }

        void configure(ConfigurationElement& config);
    };

    class DAGBASE_API RedBlackTree
    {
    public:
        RedBlackTree() = default;

        void configure(ConfigurationElement& config);

        RedBlackTreeNode* root()
        {
            return _root;
        }

        bool operator==(const RedBlackTree& other) const;

        RedBlackTreeNode* traverse(const RedBlackTreeNodePath& path);

        void insert(RedBlackTreeNodePath &path, RedBlackTreeNode* child, RedBlackTreeNode::Direction direction);

        Variant find(std::string_view path) const;
    private:
        RedBlackTreeNode* _root{&RedBlackTreeNode::NULL_NODE};
    };
}
