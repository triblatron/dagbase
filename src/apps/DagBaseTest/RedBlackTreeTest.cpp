//
// Created by Tony Horrobin on 13/10/2025.
//

#include "util/RedBlackTree.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"
#include "test/TestUtils.h"

#include <gtest/gtest.h>

class RedBlackTree_testSetColour : public ::testing::TestWithParam<std::tuple<dagbase::RedBlackTreeNode::Colour>>
{

};

TEST_P(RedBlackTree_testSetColour, testRoundTrip)
{
    auto colour = std::get<0>(GetParam());
    auto sut = new dagbase::RedBlackTreeNode(nullptr, nullptr, colour);
    EXPECT_EQ(colour, sut->colour());
    EXPECT_EQ(nullptr, sut->left());
    EXPECT_EQ(nullptr, sut->right());
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNode, RedBlackTree_testSetColour, ::testing::Values(
        std::make_tuple(dagbase::RedBlackTreeNode::COLOUR_BLACK),
        std::make_tuple(dagbase::RedBlackTreeNode::COLOUR_RED)
        ));

class RedBlackTree_testDereferenceTaggedPointer : public ::testing::TestWithParam<std::tuple<dagbase::RedBlackTreeNode::Colour>>
{

};

TEST_P(RedBlackTree_testDereferenceTaggedPointer, testExpectedColour)
{
    auto colour = std::get<0>(GetParam());
    auto child = new dagbase::RedBlackTreeNode(nullptr, nullptr, dagbase::RedBlackTreeNode::COLOUR_BLACK);
    auto root = new dagbase::RedBlackTreeNode(child, nullptr, colour);
    ASSERT_EQ(child, root->left());
    EXPECT_EQ(dagbase::RedBlackTreeNode::COLOUR_BLACK, child->colour());
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNode, RedBlackTree_testDereferenceTaggedPointer, ::testing::Values(
        std::make_tuple(dagbase::RedBlackTreeNode::COLOUR_BLACK),
        std::make_tuple(dagbase::RedBlackTreeNode::COLOUR_RED)
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
}

INSTANTIATE_TEST_SUITE_P(RedBlackTree, RedBlackTree_testInsert, ::testing::Values(
        std::make_tuple("data/tests/RedBlackTree/ChildOfRoot.lua", "root.colour", std::uint32_t{dagbase::RedBlackTreeNode::COLOUR_BLACK}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));