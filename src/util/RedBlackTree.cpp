//
// Created by Tony Horrobin on 13/10/2025.
//

#include "config/config.h"

#include "util/RedBlackTree.h"
#include "core/ConfigurationElement.h"
#include "util/Searchable.h"
#include "util/enums.h"

#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>

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
                setChild(CHILD_LEFT, &NULL_NODE);
            else
            {
                setChild(CHILD_LEFT, new RedBlackTreeNode());
                child(CHILD_LEFT)->configure(*element->child(0));
            }
            if (element->child(1)->asString() == "NULL_NODE")
                setChild(CHILD_RIGHT, &NULL_NODE);
            else
            {
                setChild(CHILD_RIGHT, new RedBlackTreeNode());
                child(CHILD_RIGHT)->configure(*element->child(1));
            }
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

        SearchableArray<std::array<const RedBlackTreeNode*,2>> a{};

        a.a[0] = left();
        a.a[1] = right();

        retval = findInternal(path, "children", a);
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

        std::ostringstream str;

        str << "colour: " << colourToString(colour()) << ", ";
        str << "direction: " << directionToString(direction()) << ", ";

        return {};
    }

    bool RedBlackTreeNode::compareChild(RedBlackTreeNode::Child index, const RedBlackTreeNode &other) const
    {
        auto myChild = child(index);
        auto otherChild = other.child(index);

        if (myChild == &NULL_NODE && otherChild != &NULL_NODE)
            return false;

        if (myChild != &NULL_NODE && otherChild == &NULL_NODE)
            return false;

        if (!(*myChild == *otherChild))
            return false;

        return true;
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

        if (!compareChild(CHILD_LEFT, other))
            return false;

        if (!compareChild(CHILD_RIGHT, other))
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

    const char *RedBlackTreeNode::childToString(RedBlackTreeNode::Child value)
    {
        switch (value)
        {
            ENUM_NAME(CHILD_LEFT)
            ENUM_NAME(CHILD_RIGHT)
        }
        return "<error>";
    }

    RedBlackTreeNode::Child RedBlackTreeNode::parseChild(const char *str)
    {
        TEST_ENUM(CHILD_LEFT, str)
        TEST_ENUM(CHILD_RIGHT, str)

        return RedBlackTreeNode::CHILD_LEFT;
    }

    void RedBlackTreeNode::setChild(RedBlackTreeNode::Child index, RedBlackTreeNode *child)
    {
        switch (index)
        {
            case CHILD_LEFT:
            {
                auto c = colour();
                auto d = direction();
                _children.a[0] = child;
                setColour(c);
                setDirection(d);
                break;
            }
            case CHILD_RIGHT:
                _children.a[1] = child;
                break;
        }
    }

    RedBlackTreeNode::Child RedBlackTreeNode::directionToChild(RedBlackTreeNode::Direction dir)
    {
        switch (dir)
        {
            case DIR_LEFT:
                return CHILD_LEFT;
            case DIR_RIGHT:
                return CHILD_RIGHT;
        }
    }

    void RedBlackTreeNode::traverse(std::function<bool(RedBlackTreeNode &)> f)
    {
        if (left() != &NULL_NODE)
        {
            left()->traverse(f);
        }

        if (right() != &NULL_NODE)
        {
            right()->traverse(f);
        }

        if (!f(*this))
        {
            return;
        }
    }

    void RedBlackTreeNode::traverse(std::function<bool(const RedBlackTreeNode &)> f) const
    {
        if (left() != &NULL_NODE)
        {
            left()->traverse(f);
        }

        if (right() != &NULL_NODE)
        {
            right()->traverse(f);
        }

        if (!f(*this))
        {
            return;
        }
    }

    std::size_t RedBlackTreeNode::countIf(std::function<bool(RedBlackTreeNode &)> f)
    {
        std::size_t count{0};

        if (left() != &NULL_NODE)
        {
            left()->countIfHelper(f, count);
        }

        if (right() != &NULL_NODE)
        {
            right()->countIfHelper(f, count);
        }

        if (f(*this))
        {
            ++count;
        }

        return count;
    }

    std::size_t RedBlackTreeNode::countIfHelper(std::function<bool(RedBlackTreeNode &)> f, std::size_t count)
    {
        if (left() != &NULL_NODE)
        {
            if (f(*left()))
                count = left()->countIfHelper(f, count+1);
            else
                count = left()->countIfHelper(f, count);
        }
        else
        {
            if (f(*left()))
            {
                ++count;
            }
        }

        if (right() != &NULL_NODE)
        {
            if (f(*right()))
                count = right()->countIfHelper(f, count+1);
            else
                count = right()->countIfHelper(f, count);
        }
        else
        {
            if (f(*right()))
            {
                ++count;
            }
        }

        if (f(*this))
        {
            ++count;
        }

        return count;
    }

    void RedBlackTreeNode::findAllPaths(RedBlackTreeNode::Path &currentPath, RedBlackTreeNode::ArrayOfPath &allPaths)
    {
        if (this == &NULL_NODE)
            return;

        currentPath.a.emplace_back(this);

        if (left() == &NULL_NODE && right() == &NULL_NODE)
        {
            allPaths.a.emplace_back(currentPath);
        }
        else
        {
            left()->findAllPaths(currentPath, allPaths);
            right()->findAllPaths(currentPath, allPaths);
        }

        currentPath.a.pop_back();
    }

    void RedBlackTreeNode::findSubPaths(const RedBlackTreeNode::ArrayOfPath &allPaths,
                                        RedBlackTreeNode::ArrayOfSubPath &subPaths)
    {
        std::size_t subPathIndex = 0;
        for (std::size_t pathIndex=0; pathIndex<allPaths.size(); ++pathIndex)
        {
            const auto& path = allPaths.a[pathIndex];
            for (std::size_t i=0; i<path.size(); ++i)
            {
                SubPath subPath;

                subPath.subPath.a.resize(path.size()-Path::difference_type(i));
                subPath.pathIndex = subPathIndex++;
                std::copy(path.begin()+Path::difference_type(i), path.end(), subPath.subPath.a.begin());
                subPaths.a.emplace_back(subPath);
            }
        }
    }

    void
    RedBlackTreeNode::findBlackNodes(const RedBlackTreeNode::ArrayOfSubPath &subPaths, RedBlackTreeNode::MapOfCount &counts)
    {
        for (const auto& path : subPaths)
        {
            if (path.pathIndex>=counts.size())
            {
                counts.a.resize(path.pathIndex+1);
            }
            counts.a[path.pathIndex].a.emplace_back(0);
            for (auto node : path.subPath)
            {
                if (node->colour() == COLOUR_BLACK)
                {
                    ++counts.a[path.pathIndex].a.back();
                }
            }
        }
    }

    PrettyPrinter &RedBlackTreeNode::print(PrettyPrinter &printer) const
    {
        printer.println("RedBlackTreeNode");
        printer.indent();
        printer.println("value:");
        printer.printIndent();
        ::operator<<(printer.str(), _value) << '\n';
        printer.println("colour: " + std::string(colourToString(colour())));
        printer.println("direction: " + std::string(directionToString(direction())));
        printer.println("leftChild:");
        if (left()!=&NULL_NODE)
        {
            printer.indent();
            left()->print(printer);
            printer.outdent();
        }
        else
            printer.println("NULL_NODE");
        printer.println("rightChild:");
        if (right()!=&NULL_NODE)
        {
            printer.indent();
            right()->print(printer);
            printer.outdent();
        }
        else
            printer.println("NULL_NODE");
        printer.outdent();
        printer.println("End");
        return printer;
    }

    void RedBlackTreeNodePath::configure(ConfigurationElement &config)
    {
        config.eachChild([this](ConfigurationElement& child) {
            auto i = child.asString();
            path.emplace_back(RedBlackTreeNode::parseChild(i.c_str()));

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
        RedBlackTreeNode* parent = _root;

        for (auto index : path.path)
        {
            parent = parent->child(index);
        }
        return parent;
    }

    RedBlackTreeNode * RedBlackTree::rotateSubtree(RedBlackTreeNodePath &path, std::size_t numGenerations, RedBlackTreeNode *sub,
                                                   RedBlackTreeNode::Direction direction)
    {
        RedBlackTreeNode* subParent = path.ancestor(_root, numGenerations+1);
        RedBlackTreeNode* newRoot = sub->otherChild(direction);
        RedBlackTreeNode* newChild = newRoot->child(RedBlackTreeNode::directionToChild(direction));

        sub->setChild(RedBlackTreeNode::directionToChild(RedBlackTreeNode::oppositeDirection(direction)), newChild);
        //newRoot->parent = subParent;
        newRoot->setChild(RedBlackTreeNode::directionToChild(direction), sub);
        //sub->parent = newRoot
        if (subParent != &RedBlackTreeNode::NULL_NODE)
        {
            if (sub == subParent->child(RedBlackTreeNode::CHILD_RIGHT))
            {
                subParent->setChild(RedBlackTreeNode::CHILD_RIGHT, newRoot);
            }
            else
            {
                subParent->setChild(RedBlackTreeNode::CHILD_LEFT, newRoot);
            }
        }
        else
        {
            _root = newRoot;
        }

        return newRoot;
    }

    void RedBlackTree::insert(RedBlackTreeNodePath &path, RedBlackTreeNode *node, RedBlackTreeNode::Direction direction)
    {
        node->setColour(RedBlackTreeNode::COLOUR_RED);
        auto parent = path.parent(_root);
        if (parent == &RedBlackTreeNode::NULL_NODE)
        {
            _root = node;
            return;
        }

        parent->setChild(RedBlackTreeNode::directionToChild(direction), node);

        do
        {
            // Case #1
            if (parent->colour() == RedBlackTreeNode::COLOUR_BLACK)
                return;

            auto grandparent = path.grandparent(_root);

            if (grandparent == &RedBlackTreeNode::NULL_NODE)
            {
                // Case #4
                parent->setColour(RedBlackTreeNode::COLOUR_BLACK);
                return;
            }
            direction = parent->direction();
            RedBlackTreeNode* uncle = grandparent->otherChild(direction);
            if (uncle == &RedBlackTreeNode::NULL_NODE || uncle->colour() == RedBlackTreeNode::COLOUR_BLACK)
            {
                if (node == parent->otherChild(direction))
                {
                    // Case #5
                    rotateSubtree(path, 1, parent, direction);
                    node = parent;
                    parent = grandparent->child(RedBlackTreeNode::directionToChild(direction));
                }
                // Case #6
                rotateSubtree(path, 2, grandparent, RedBlackTreeNode::oppositeDirection(direction));
                parent->setColour(RedBlackTreeNode::COLOUR_BLACK);
                grandparent->setColour(RedBlackTreeNode::COLOUR_RED);

                return;
            }
            // Case #2
            parent->setColour(RedBlackTreeNode::COLOUR_BLACK);
            uncle->setColour(RedBlackTreeNode::COLOUR_BLACK);
            grandparent->setColour(RedBlackTreeNode::COLOUR_RED);

            parent = path.parent(_root, 2);
        } while (parent != &RedBlackTreeNode::NULL_NODE);

        // Case #3
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

    RedBlackTree::ValidationResult RedBlackTree::validate() const
    {
        // Property #1 is satisfied by Colour.
        // All nodes are either red or black
        // Property #2 is satisfied by NULL_NODE being black.
        // Property #3:A red node does not have a red child.
        ValidationResult valid = VALIDATION_OK;
        _root->traverse([&valid](RedBlackTreeNode& child) {
            if (child.colour() == RedBlackTreeNode::COLOUR_RED &&
                    (child.left()->colour() == RedBlackTreeNode::COLOUR_RED || child.right()->colour() == RedBlackTreeNode::COLOUR_RED)
                    )
            {
                valid = VALIDATION_RED_HAS_RED_CHILD;
                return false;
            }
            if (child.numChildren() == 1 &&
                    ((child.left() != &RedBlackTreeNode::NULL_NODE && child.left()->colour() == RedBlackTreeNode::COLOUR_BLACK) ||
                    (child.right() != &RedBlackTreeNode::NULL_NODE && child.right()->colour() == RedBlackTreeNode::COLOUR_BLACK))
                    )
            {
                valid = VALIDATION_ONE_CHILD_BLACK;
                return false;
            }
            return true;
        });
        if (valid != VALIDATION_OK)
            return valid;
        // Property 4:All paths from a given node to a leaf have the same number of black nodes.
        RedBlackTreeNode::Path currentPath;
        RedBlackTreeNode::ArrayOfPath allPaths;
        _root->findAllPaths(currentPath, allPaths);
        RedBlackTreeNode::ArrayOfSubPath subPaths;
        RedBlackTreeNode::findSubPaths(allPaths, subPaths);
        RedBlackTreeNode::MapOfCount counts;
        RedBlackTreeNode::findBlackNodes(subPaths, counts);
        std::map<RedBlackTreeNode*, std::vector<std::uint32_t>> countsPerNode;

        for (auto path : subPaths)
        {
            auto it=countsPerNode.find(path.subPath.a[0]);

            if ( it==countsPerNode.end())
            {
                auto p = countsPerNode.emplace(path.subPath.a[0], std::vector<std::uint32_t>());
                it = p.first;
            }
            it->second.emplace_back(counts.a[path.pathIndex].a[0]);
        }

        for (const auto& countsForNode : countsPerNode)
        {
            if (!countsForNode.second.empty())
            {
                auto firstCount = countsForNode.second[0];
                for (std::size_t index = 1; index < countsForNode.second.size(); ++index)
                {
                    if (firstCount != countsForNode.second[index])
                    {
                        valid = VALIDATION_BLACK_HEIGHT_VIOLATION;
                    }
                }
            }
        }
        return valid;
    }

    const char *RedBlackTree::validationResultToString(RedBlackTree::ValidationResult value)
    {
        switch (value)
        {
            ENUM_NAME(VALIDATION_OK)
            ENUM_NAME(VALIDATION_ONE_CHILD_BLACK)
            ENUM_NAME(VALIDATION_RED_HAS_RED_CHILD)
            ENUM_NAME(VALIDATION_BLACK_HEIGHT_VIOLATION)
        }
        return "<error>";
    }

    Variant RedBlackTreeNode::SubPath::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "pathIndex", std::uint32_t(pathIndex));
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "subPath", subPath);
        if (retval.has_value())
            return retval;

        return {};
    }
}

std::ostream& operator<<(std::ostream& str, const dagbase::RedBlackTree& value)
{
    dagbase::PrettyPrinter printer(str,2);
    printer.println("RedBlackTree");
    printer.indent();
    value.root()->print(printer);
    printer.outdent();

    return str;
}