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
        if (expected.value()->index() == dagbase::Variant::TYPE_DOUBLE && actual.value()->index() == dagbase::Variant::TYPE_DOUBLE)
        {
            EXPECT_NEAR(actual.asDouble(), expected.asDouble(), tolerance);
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
        EXPECT_NE(expected, actual);
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