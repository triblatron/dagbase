//
// Created by Tony Horrobin on 06/04/2025.
//

#include "test/TestUtils.h"

#include <gtest/gtest.h>

void assertComparison(dagbase::ConfigurationElement::ValueType expected, dagbase::ConfigurationElement::ValueType actual, double tolerance, dagbase::ConfigurationElement::RelOp op)
{
    switch (op)
    {
    case dagbase::ConfigurationElement::RELOP_EQ:
        if (expected->index() == dagbase::ConfigurationElement::TYPE_DOUBLE && actual->index() == dagbase::ConfigurationElement::TYPE_DOUBLE)
        {
            EXPECT_NEAR(std::get<double>(actual.value()), std::get<double>(expected.value()), tolerance);
        }
        else if (expected.has_value() && actual.has_value())
        {
            EXPECT_EQ(expected.value(), actual.value());
        }
        else if ((!expected.has_value() && actual.has_value()) || (expected.has_value() && !actual.has_value()))
        {
            FAIL();
        }
        break;
    case dagbase::ConfigurationElement::RELOP_NE:
        EXPECT_NE(expected.value(), actual.value());
        break;
    case dagbase::ConfigurationElement::RELOP_LT:
        EXPECT_LT(actual.value(), expected.value());
        break;
    case dagbase::ConfigurationElement::RELOP_LE:
        EXPECT_LE(actual.value(), expected.value());
        break;
    case dagbase::ConfigurationElement::RELOP_GT:
        EXPECT_GT(actual.value(), expected.value());
        break;
    case dagbase::ConfigurationElement::RELOP_GE:
        EXPECT_GE(actual.value(), expected.value());
        break;
    default:
        assert(false);
    }
}