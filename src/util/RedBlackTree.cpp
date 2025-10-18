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
    RedBlackTreeNode RedBlackTreeNode::NULL_NODE(&NULL_NODE, &NULL_NODE, RedBlackTreeNode::COLOUR_BLACK, RedBlackTreeNode::DIR_LEFT);

    RedBlackTreeNode::RedBlackTreeNode(RedBlackTreeNode *left, RedBlackTreeNode *right, RedBlackTreeNode::Colour colour, Direction dir)
    {
        _children.a[CHILD_LEFT] = left;
        _children.a[CHILD_RIGHT] = right;
        setColour(colour);
        setDirection(dir);
    }

    RedBlackTreeNode::RedBlackTreeNode()
    {
        _children.a[CHILD_LEFT] = &NULL_NODE;
        _children.a[CHILD_RIGHT] = &NULL_NODE;
    }

    void RedBlackTreeNode::configure(ConfigurationElement &config)
    {
        ConfigurationElement::readConfig(config, "value", &_value);
        Colour c{COLOUR_BLACK};
        ConfigurationElement::readConfig<Colour>(config, "colour", &parseColour, &c);
        setColour(c);
        Direction dir{DIR_LEFT};
        ConfigurationElement::readConfig<Direction>(config, "direction", &parseDirection, &dir);
        setDirection(dir);
        if (auto element=config.findElement("children"); element && element->numChildren()==2)
        {
            if (element->child(0)->asString() == "NULL_NODE")
                _children.a[0] = &NULL_NODE;
            else
                _children.a[0]->configure(*element->child(0));

            if (element->child(1)->asString() == "NULL_NODE")
                _children.a[1] = &NULL_NODE;
            else
                _children.a[1]->configure(*element->child(1));
        }
    }

    Variant RedBlackTreeNode::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "value", _value);
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "colour", std::uint32_t(colour()));
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "direction", std::uint32_t(direction()));
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

        if (_value != other._value)
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

    const char *RedBlackTreeNode::directionToString(RedBlackTreeNode::Direction value)
    {
        switch (value)
        {
            ENUM_NAME(DIR_LEFT)
            ENUM_NAME(DIR_RIGHT)
        }

        return "<error>";
    }

    RedBlackTreeNode::Direction RedBlackTreeNode::parseDirection(const char *str)
    {
        TEST_ENUM(DIR_LEFT, str);
        TEST_ENUM(DIR_RIGHT, str);

        return DIR_LEFT;
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
            if (element->asString() == "NULL_NODE")
            {
                _root = &RedBlackTreeNode::NULL_NODE;
            }
            else
            {
                _root = new RedBlackTreeNode();
                _root->configure(*element);
            }

        }
    }

    RedBlackTreeNode *RedBlackTree::traverse(const RedBlackTreeNodePath &path)
    {
        return _root;
    }

    void RedBlackTree::insert(RedBlackTreeNode *parent, RedBlackTreeNode *child)
    {
        if (_root == &RedBlackTreeNode::NULL_NODE)
            _root = child;
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