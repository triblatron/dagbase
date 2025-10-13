//
// Created by Tony Horrobin on 13/10/2025.
//

#include "util/RedBlackTree.h"

#include <gtest/gtest.h>

class RedBlackTree_testSetColour : public ::testing::TestWithParam<std::tuple<dagbase::RedBlackTree::Colour>>
{

};

TEST_P(RedBlackTree_testSetColour, testRoundTrip)
{
    auto colour = std::get<0>(GetParam());
    auto sut = new dagbase::RedBlackTree(nullptr, nullptr, colour);
    EXPECT_EQ(colour, sut->colour());
    EXPECT_EQ(nullptr, sut->left());
    EXPECT_EQ(nullptr, sut->right());
}

INSTANTIATE_TEST_SUITE_P(RedBlackTree, RedBlackTree_testSetColour, ::testing::Values(
        std::make_tuple(dagbase::RedBlackTree::COLOUR_BLACK),
        std::make_tuple(dagbase::RedBlackTree::COLOUR_RED)
        ));