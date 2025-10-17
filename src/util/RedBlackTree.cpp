//
// Created by Tony Horrobin on 13/10/2025.
//

#include "config/config.h"

#include "util/RedBlackTree.h"
#include "core/ConfigurationElement.h"
#include "util/Searchable.h"

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

    void RedBlackTreeNodePath::configure(ConfigurationElement &config)
    {
        config.eachChild([this](ConfigurationElement& child) {
            path.emplace_back(child.asInteger());

            return true;
        });
    }

    void RedBlackTree::configure(ConfigurationElement &config)
    {

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
}