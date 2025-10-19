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
    auto direction = dagbase::RedBlackTreeNode::DIR_LEFT;
    dagbase::ConfigurationElement::readConfig<dagbase::RedBlackTreeNode::Direction>(*config, "direction", &dagbase::RedBlackTreeNode::parseDirection, &direction);
    sut->insert(path, child, direction);
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
    EXPECT_TRUE(sut->validate());
}

INSTANTIATE_TEST_SUITE_P(RedBlackTree, RedBlackTree_testInsert, ::testing::Values(
        std::make_tuple("data/tests/RedBlackTree/ReplaceRoot.lua", "root.value", std::int64_t{10}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/ReplaceRoot.lua", "root.colour", std::uint32_t{dagbase::RedBlackTreeNode::COLOUR_RED}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/ReplaceRoot.lua", "root.direction", std::uint32_t{dagbase::RedBlackTreeNode::DIR_LEFT}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/ReplaceRoot.lua", "root.children[0].toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/ReplaceRoot.lua", "root.children[1].toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/LeftChild.lua", "root.children[0].value", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/LeftChild.lua", "root.children[0].colour", std::uint32_t{dagbase::RedBlackTreeNode::COLOUR_RED}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/LeftChild.lua", "root.children[0].direction", std::uint32_t{dagbase::RedBlackTreeNode::DIR_LEFT}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/LeftChild.lua", "root.children[0].toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_NE)
        ));

class RedBlackTreeNodeColour_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::RedBlackTreeNode::Colour>>
{

};

TEST_P(RedBlackTreeNodeColour_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::RedBlackTreeNode::colourToString(value));
    EXPECT_EQ(value, dagbase::RedBlackTreeNode::parseColour(str));
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNode, RedBlackTreeNodeColour_testRoundTrip, ::testing::Values(
        std::make_tuple("COLOUR_BLACK", dagbase::RedBlackTreeNode::COLOUR_BLACK),
        std::make_tuple("COLOUR_RED", dagbase::RedBlackTreeNode::COLOUR_RED)
        ));

class RedBlackreeNodeDirection_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::RedBlackTreeNode::Direction>>
{

};

TEST_P(RedBlackreeNodeDirection_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::RedBlackTreeNode::directionToString(value));
    EXPECT_EQ(value, dagbase::RedBlackTreeNode::parseDirection(str));
}

INSTANTIATE_TEST_SUITE_P(RedBlackreeNodeDirection, RedBlackreeNodeDirection_testRoundTrip, ::testing::Values(
        std::make_tuple("DIR_LEFT", dagbase::RedBlackTreeNode::DIR_LEFT),
        std::make_tuple("DIR_RIGHT", dagbase::RedBlackTreeNode::DIR_RIGHT)
        ));

class RedBlackTreeNode_testOppositeDirection : public ::testing::TestWithParam<std::tuple<dagbase::RedBlackTreeNode::Direction, dagbase::RedBlackTreeNode::Direction>>
{

};

TEST_P(RedBlackTreeNode_testOppositeDirection, testOpposite)
{
    auto dir = std::get<0>(GetParam());
    auto opposite = std::get<1>(GetParam());
    EXPECT_EQ(opposite, dagbase::RedBlackTreeNode::oppositeDirection(dir));
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNode, RedBlackTreeNode_testOppositeDirection, ::testing::Values(
        std::make_tuple(dagbase::RedBlackTreeNode::DIR_LEFT, dagbase::RedBlackTreeNode::DIR_RIGHT),
        std::make_tuple(dagbase::RedBlackTreeNode::DIR_RIGHT, dagbase::RedBlackTreeNode::DIR_LEFT)
        ));

class RedBlackTreeNodeChild_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::RedBlackTreeNode::Child>>
{

};

TEST_P(RedBlackTreeNodeChild_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::RedBlackTreeNode::childToString(value));
    EXPECT_EQ(value, dagbase::RedBlackTreeNode::parseChild(str));
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNodeChild, RedBlackTreeNodeChild_testRoundTrip, ::testing::Values(
        std::make_tuple("CHILD_LEFT", dagbase::RedBlackTreeNode::CHILD_LEFT),
        std::make_tuple("CHILD_RIGHT", dagbase::RedBlackTreeNode::CHILD_RIGHT)
        ));

class RedBlackTreeNodePath_testAncestor : public ::testing::TestWithParam<std::tuple<const char*, std::size_t, std::size_t, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(RedBlackTreeNodePath_testAncestor, testExpectedProperties)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::RedBlackTreeNodePath sut;
    if (auto element=config->findElement("path"); element)
    {
        sut.configure(*element);
    }
    auto tree = std::make_unique<dagbase::RedBlackTree>();
    if (auto element = config->findElement("tree"); element)
    {
        tree->configure(*element);
    }
    auto numGenerations = std::get<1>(GetParam());
    auto startGeneration = std::get<2>(GetParam());
    auto path = std::get<3>(GetParam());
    auto value = std::get<4>(GetParam());
    auto tolerance = std::get<5>(GetParam());
    auto op = std::get<6>(GetParam());
    auto child = tree->traverse(sut);
    ASSERT_NE(nullptr, child);
    auto ancestor = sut.ancestor(tree->root(), numGenerations, startGeneration);
    ASSERT_NE(nullptr, ancestor);
    auto actual = ancestor->find(path);
    assertComparison(value, actual, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNodePath, RedBlackTreeNodePath_testAncestor, ::testing::Values(
        std::make_tuple("data/tests/RedBlackTree/Empty.lua", std::size_t{0}, std::size_t{0}, "toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Empty.lua", std::size_t{1}, std::size_t{0}, "toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Empty.lua", std::size_t{2}, std::size_t{0}, "toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", std::size_t{0}, std::size_t{0}, "toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_NE),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", std::size_t{0}, std::size_t{0}, "value", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", std::size_t{1}, std::size_t{0}, "toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", std::size_t{2}, std::size_t{0}, "toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Child.lua", std::size_t{0}, std::size_t{0}, "toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Child.lua", std::size_t{1}, std::size_t{0}, "toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_NE),
        std::make_tuple("data/tests/RedBlackTree/Child.lua", std::size_t{1}, std::size_t{0}, "value", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", std::size_t{0}, std::size_t{0}, "value", std::int64_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", std::size_t{1}, std::size_t{0}, "value", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", std::size_t{2}, std::size_t{0}, "value", std::int64_t{10}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", std::size_t{1}, std::size_t{1}, "value", std::int64_t{10}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", std::size_t{1}, std::size_t{2}, "toString", std::string("NULL_NODE"), 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

class RedBlackTreeNode_testFindAllPaths : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(RedBlackTreeNode_testFindAllPaths, testExpectedProperties)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto sut = std::make_unique<dagbase::RedBlackTree>();
    auto treeConfig = config->findElement("tree");
    ASSERT_NE(nullptr, treeConfig);
    sut->configure(*treeConfig);
    dagbase::RedBlackTreeNode::Path currentPath;
    dagbase::RedBlackTreeNode::ArrayOfPath allPaths;
    sut->root()->findAllPaths(currentPath, allPaths);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actual = allPaths.find(path);
    assertComparison(value, actual, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNode, RedBlackTreeNode_testFindAllPaths, ::testing::Values(
        std::make_tuple("data/tests/RedBlackTree/Empty.lua", "size", std::uint32_t{0}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", "size", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", "size", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", "[0].size", std::uint32_t{3}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/MultipleChildren.lua", "size", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/MultipleChildren.lua", "[0].size", std::uint32_t{3}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/MultipleChildren.lua", "[1].size", std::uint32_t{3}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

class RedBlackTreeNode_testFindSubPaths : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(RedBlackTreeNode_testFindSubPaths, testExpectedProperties)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto sut = std::make_unique<dagbase::RedBlackTree>();
    auto treeConfig = config->findElement("tree");
    ASSERT_NE(nullptr, treeConfig);
    sut->configure(*treeConfig);
    dagbase::RedBlackTreeNode::Path currentPath;
    dagbase::RedBlackTreeNode::ArrayOfPath allPaths;
    sut->root()->findAllPaths(currentPath, allPaths);
    dagbase::RedBlackTreeNode::ArrayOfSubPath subPaths;
    dagbase::RedBlackTreeNode::findSubPaths(allPaths, subPaths);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actual = subPaths.find(path);
    assertComparison(value, actual, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNode, RedBlackTreeNode_testFindSubPaths, ::testing::Values(
        std::make_tuple("data/tests/RedBlackTree/Empty.lua", "size", std::uint32_t{0}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", "size", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", "[0].subPath.size", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", "[0].subPath[0].value", std::int64_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", "size", std::uint32_t{3}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", "[0].subPath.size", std::uint32_t{3}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", "[1].subPath.size", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", "[2].subPath.size", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/MultipleChildren.lua", "size", std::uint32_t{6}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/MultipleChildren.lua", "[0].subPath.size", std::uint32_t{3}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/MultipleChildren.lua", "[1].subPath.size", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/MultipleChildren.lua", "[2].subPath.size", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/MultipleChildren.lua", "[3].subPath.size", std::uint32_t{3}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/MultipleChildren.lua", "[4].subPath.size", std::uint32_t{2}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/MultipleChildren.lua", "[5].subPath.size", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));

class RedBlackTreeNode_testFindBlackNodes : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(RedBlackTreeNode_testFindBlackNodes, testExpectedProperties)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    auto sut = std::make_unique<dagbase::RedBlackTree>();
    auto treeConfig = config->findElement("tree");
    ASSERT_NE(nullptr, treeConfig);
    sut->configure(*treeConfig);
    dagbase::RedBlackTreeNode::Path currentPath;
    dagbase::RedBlackTreeNode::ArrayOfPath allPaths;
    sut->root()->findAllPaths(currentPath, allPaths);
    dagbase::RedBlackTreeNode::ArrayOfSubPath subPaths;
    dagbase::RedBlackTreeNode::findSubPaths(allPaths, subPaths);
    dagbase::RedBlackTreeNode::MapOfCount counts;
    dagbase::RedBlackTreeNode::findBlackNodes(subPaths, counts);
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());
    auto actual = counts.find(path);
    assertComparison(value, actual, tolerance, op);
}

INSTANTIATE_TEST_SUITE_P(RedBlackTreeNode, RedBlackTreeNode_testFindBlackNodes, ::testing::Values(
        std::make_tuple("data/tests/RedBlackTree/Empty.lua", "size", std::uint32_t{0}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", "size", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", "[0].size", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/Root.lua", "[0].[0]", std::uint32_t{1}, 0.0, dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("data/tests/RedBlackTree/NestedChild.lua", "size", std::uint32_t{3}, 0.0, dagbase::ConfigurationElement::RELOP_EQ)
        ));