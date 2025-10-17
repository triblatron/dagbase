//
// Created by Tony Horrobin on 13/10/2025.
//

#include "config/config.h"

#include "util/RedBlackTree.h"
#include "core/ConfigurationElement.h"
#include "util/Searchable.h"
#include "util/enums.h"

namespace dagbase
{
    RedBlackTreeNode RedBlackTreeNode::NULL_NODE(&NULL_NODE, &NULL_NODE, RedBlackTreeNode::COLOUR_BLACK);

    RedBlackTreeNode::RedBlackTreeNode(RedBlackTreeNode *left, RedBlackTreeNode *right, RedBlackTreeNode::Colour colour)
    {
        _children.a[CHILD_LEFT] = left;
        _children.a[CHILD_RIGHT] = right;
        setColour(colour);
    }

    RedBlackTreeNode::RedBlackTreeNode()
    {
        _children.a[CHILD_LEFT] = &NULL_NODE;
        _children.a[CHILD_RIGHT] = &NULL_NODE;
    }

    void RedBlackTreeNode::configure(ConfigurationElement &config)
    {
        Colour c{COLOUR_BLACK};
        ConfigurationElement::readConfig<Colour>(config, "colour", &parseColour, &c);
        setColour(c);
    }

    Variant RedBlackTreeNode::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "colour", std::uint32_t(colour()));
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "children", _children);
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "toString", Variant(toString()));
        if (retval.has_value())
            return retval;

        return {};
    }

    std::string RedBlackTreeNode::toString() const
    {
        if (this == &NULL_NODE)
        {
            return "NULL_NODE";
        }

        return {};
    }

    bool RedBlackTreeNode::operator==(const RedBlackTreeNode &other) const
    {
        // Prevent infinite recursion due to NULL_NODE pointing to itself.
        if (this == &other)
            return true;

        if (colour() != other.colour())
            return false;

        if (_children.a[CHILD_LEFT] == &NULL_NODE && other._children.a[CHILD_LEFT] != &NULL_NODE)
            return false;

        if (_children.a[CHILD_LEFT] != &NULL_NODE && other._children.a[CHILD_LEFT] == &NULL_NODE)
            return false;

        if (!(*_children.a[CHILD_LEFT] == *other._children.a[CHILD_LEFT]))
            return false;

        if (_children.a[CHILD_RIGHT] == &NULL_NODE && other._children.a[CHILD_RIGHT] != &NULL_NODE)
            return false;

        if (_children.a[CHILD_RIGHT] != &NULL_NODE && other._children.a[CHILD_RIGHT] == &NULL_NODE)
            return false;

        if (!(*_children.a[CHILD_RIGHT] == *other._children.a[CHILD_RIGHT]))
            return false;

        return true;
    }

    RedBlackTreeNode::Colour RedBlackTreeNode::parseColour(const char *str)
    {
        TEST_ENUM(COLOUR_BLACK, str);
        TEST_ENUM(COLOUR_RED, str);

        return RedBlackTreeNode::COLOUR_BLACK;
    }

    const char *RedBlackTreeNode::colourToString(RedBlackTreeNode::Colour value)
    {
        switch (value)
        {
            ENUM_NAME(COLOUR_BLACK)
            ENUM_NAME(COLOUR_RED)
        }
        return "<error>";
    }

    void RedBlackTreeNodePath::configure(ConfigurationElement &config)
    {
        config.eachChild([this](ConfigurationElement& child) {
            path.emplace_back(child.asInteger());

            return true;
        });
    }

    void RedBlackTree::configure(ConfigurationElement &config)
    {
        if (auto element=config.findElement("root"); element)
        {
            _root->configure(*element);
        }
    }

    RedBlackTreeNode *RedBlackTree::traverse(const RedBlackTreeNodePath &path)
    {
        return _root;
    }

    void RedBlackTree::insert(RedBlackTreeNode *parent, RedBlackTreeNode *child)
    {

    }

    Variant RedBlackTree::find(std::string_view path) const
    {
        Variant retval;

        retval = findInternal(path, "root", *_root);
        if (retval.has_value())
            return retval;

        return {};
    }

    bool RedBlackTree::operator==(const RedBlackTree &other) const
    {
        return *_root == *other._root;
    }
}