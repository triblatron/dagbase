//
// Created by Tony Horrobin on 13/10/2025.
//

#include "util/RedBlackTree.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"
#include "test/TestUtils.h"

#include <gtest/gtest.h>

class RedBlackTree_testSetFlags : public ::testing::TestWithParam<std::tuple<dagbase::RedBlackTreeNode::Colour, dagbase::RedBlackTreeNode::Direction>>
{

};

TEST_P(RedBlackTree_testSetFlags, testRoundTrip)
{
    auto colour = std::get<0>(GetParam());
    auto dir = std::get<1>(GetParam());
    auto sut = new dagbase::RedBlackTreeNode(&dagbase::RedBlackTreeNode::NULL_NODE, &dagbase::RedBlackTreeNode::NULL_NODE, colour, dir);
    EXPECT_EQ(colour, sut->colour());
    EXPECT_EQ(dir, sut->direction());
    EXPECT_EQ(&dagbase::RedBlackTreeNode::NULL_NODE, sut->left());
    EXPECT_EQ(&dagbase::RedBlackTreeNode::NULL_NODE, sut->right());
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNode, RedBlackTree_testSetFlags, ::testing::Values(
        std::make_tuple(dagbase::RedBlackTreeNode::COLOUR_BLACK, dagbase::RedBlackTreeNode::DIR_LEFT),
        std::make_tuple(dagbase::RedBlackTreeNode::COLOUR_BLACK, dagbase::RedBlackTreeNode::DIR_RIGHT),
        std::make_tuple(dagbase::RedBlackTreeNode::COLOUR_RED, dagbase::RedBlackTreeNode::DIR_LEFT),
        std::make_tuple(dagbase::RedBlackTreeNode::COLOUR_RED, dagbase::RedBlackTreeNode::DIR_RIGHT)
        ));

class RedBlackTree_testDereferenceTaggedPointer : public ::testing::TestWithParam<std::tuple<dagbase::RedBlackTreeNode::Colour, dagbase::RedBlackTreeNode::Direction>>
{

};

TEST_P(RedBlackTree_testDereferenceTaggedPointer, testExpectedColour)
{
    auto colour = std::get<0>(GetParam());
    auto dir = std::get<1>(GetParam());
    auto child = new dagbase::RedBlackTreeNode(&dagbase::RedBlackTreeNode::NULL_NODE, &dagbase::RedBlackTreeNode::NULL_NODE, dagbase::RedBlackTreeNode::COLOUR_BLACK, dagbase::RedBlackTreeNode::DIR_LEFT);
    auto root = new dagbase::RedBlackTreeNode(child, &dagbase::RedBlackTreeNode::NULL_NODE, colour, dir);
    ASSERT_EQ(child, root->left());
    EXPECT_EQ(dagbase::RedBlackTreeNode::COLOUR_BLACK, child->colour());
    EXPECT_EQ(dagbase::RedBlackTreeNode::DIR_LEFT, child->direction());
    EXPECT_EQ(colour, root->colour());
    EXPECT_EQ(dir, root->direction());
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNode, RedBlackTree_testDereferenceTaggedPointer, ::testing::Values(
        std::make_tuple(dagbase::RedBlackTreeNode::COLOUR_BLACK, dagbase::RedBlackTreeNode::DIR_LEFT),
        std::make_tuple(dagbase::RedBlackTreeNode::COLOUR_RED, dagbase::RedBlackTreeNode::DIR_RIGHT)
        ));

class RedBlackTree_testInsert : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{
public:
};

TEST_P(RedBlackTree_testInsert, testExpectedProperties)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto sut = std::make_unique<dagbase::RedBlackTree>();
    auto treeConfig = config->findElement("tree");
    ASSERT_NE(nullptr, treeConfig);
    sut->configure(*treeConfig);
    auto nodePathStr = std::get<1>(GetParam());
    auto nodePathConfig = config->findElement("path");
    ASSERT_NE(nullptr, nodePathConfig);
    dagbase::RedBlackTreeNodePath path;
    path.configure(*nodePathConfig);
    auto parent = sut->traverse(path);
    ASSERT_NE(nullptr, parent);
    auto childConfig = config->findElement("child");
    ASSERT_NE(nullptr, childConfig);
    auto child = new dagbase::RedBlackTreeNode();
    child->configure(*childConfig);
    sut->insert(parent, child);
    auto attrPath = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actual = sut->find(attrPath);
    assertComparison(value, actual, tolerance, op);
    auto afterConfig = config->findElement("after");
    ASSERT_NE(nullptr, afterConfig);
    auto afterTree = std::make_unique<dagbase::RedBlackTree>();
    afterTree->configure(*afterConfig);
    EXPECT_EQ(*afterTree, *sut);
}

INSTANTIATE_TEST_SUITE_P(RedBlackTree, RedBlackTree_testInsert, ::testing::Values(
        std::make_tuple("data/tests/RedBlackTree/ReplaceRoot.lua", "root.colour", std::uint32_t{dagbase::RedBlackTreeNode::COLOUR_BLACK}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/ReplaceRoot.lua", "root.children[0].toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/ReplaceRoot.lua", "root.children[1].toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

class RedBlackTreeNode_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::RedBlackTreeNode::Colour>>
{

};

TEST_P(RedBlackTreeNode_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::RedBlackTreeNode::colourToString(value));
    EXPECT_EQ(value, dagbase::RedBlackTreeNode::parseColour(str));
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNode, RedBlackTreeNode_testRoundTrip, ::testing::Values(
        std::make_tuple("COLOUR_BLACK", dagbase::RedBlackTreeNode::COLOUR_BLACK),
        std::make_tuple("COLOUR_RED", dagbase::RedBlackTreeNode::COLOUR_RED)
        ));
