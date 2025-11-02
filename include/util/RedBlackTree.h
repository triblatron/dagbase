//
// Created by Tony Horrobin on 13/10/2025.
//

#pragma once

#include "config/DagBaseExport.h"
#include "core/Variant.h"
#include "util/SearchableArray.h"
#include "util/PrettyPrinter.h"

#include <array>
#include <cstdint>
#include <vector>
#include <string_view>
#include <iosfwd>

namespace dagbase
{
    class ConfigurationElement;
    struct RedBlackTreeNodePath;

    class DAGBASE_API RedBlackTreeNode
    {
    public:
        using Path = SearchableArray<std::vector<RedBlackTreeNode*>>;
        using ArrayOfPath = SearchableArray<std::vector<Path>>;
        struct DAGBASE_API SubPath
        {
            std::size_t pathIndex{0};
            Path subPath;

            Variant find(std::string_view path) const;
        };
        using ArrayOfSubPath = SearchableArray<std::vector<SubPath>>;
        using MapOfCount = SearchableArray<std::vector<SearchablePrimitiveArray<std::vector<std::uint32_t>>>>;
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

        std::size_t numChildren() const
        {
            std::size_t retval = 0;

            if (left() != &NULL_NODE)
                ++retval;

            if (right() != &NULL_NODE)
                ++retval;

            return retval;
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

            return nullptr;
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

            return nullptr;
        }

        RedBlackTreeNode* otherChild(Direction dir)
        {
            auto childIndex = directionToChild(oppositeDirection(dir));

            return child(childIndex);
        }

        void traverse(std::function<bool(RedBlackTreeNode&)> f);

        void traverse(std::function<bool(const RedBlackTreeNode&)> f) const;

        void findAllPaths(Path& currentPath, ArrayOfPath & allPaths);

        std::size_t countIf(std::function<bool(RedBlackTreeNode&)> f);

        bool operator==(const RedBlackTreeNode& other) const;

        Variant find(std::string_view path) const;

        std::string toString() const;

        PrettyPrinter& print(PrettyPrinter& printer) const;

        static void findSubPaths(const ArrayOfPath& allPaths, ArrayOfSubPath & subPaths);

        static void findBlackNodes(const ArrayOfSubPath& subPaths, MapOfCount& counts);

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
        std::size_t countIfHelper(std::function<bool(RedBlackTreeNode&)> f, std::size_t count);
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

        RedBlackTreeNode *ancestor(RedBlackTreeNode *root, std::size_t numGenerations, std::size_t startGeneration=0)
        {
            RedBlackTreeNode* retval = root;

            if (path.size()>=startGeneration+numGenerations)
                for (std::size_t pathIndex = startGeneration; pathIndex<path.size() - numGenerations; ++pathIndex)
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

        RedBlackTreeNode *parent(RedBlackTreeNode *root, std::size_t startGeneration=0)
        {
            return ancestor(root, 1, startGeneration);
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
        enum ValidationResult : std::uint32_t
        {
            VALIDATION_OK,
            VALIDATION_ONE_CHILD_BLACK,
            VALIDATION_RED_HAS_RED_CHILD,
            VALIDATION_BLACK_HEIGHT_VIOLATION
        };

    public:
        RedBlackTree() = default;

        void configure(ConfigurationElement& config);

        RedBlackTreeNode* root()
        {
            return _root;
        }

        const RedBlackTreeNode* root() const
        {
            return _root;
        }

        bool operator==(const RedBlackTree& other) const;

        RedBlackTreeNode* traverse(const RedBlackTreeNodePath& path);

        void insert(RedBlackTreeNodePath &path, RedBlackTreeNode* node, RedBlackTreeNode::Direction direction);

        Variant find(std::string_view path) const;

        ValidationResult validate() const;

        static const char* validationResultToString(ValidationResult value);
    private:
        RedBlackTreeNode* rotateSubtree(RedBlackTreeNodePath &path, std::size_t numGenerations, RedBlackTreeNode *sub, RedBlackTreeNode::Direction direction);
        RedBlackTreeNode* _root{&RedBlackTreeNode::NULL_NODE};
    };
}

std::ostream DAGBASE_API & operator<<(std::ostream& str, const dagbase::RedBlackTree& value);