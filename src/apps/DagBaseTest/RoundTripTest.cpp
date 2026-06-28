//
// Created by Tony Horrobin on 27/09/2025.
//

#include "core/Variant.h"
#include "core/ConfigurationElement.h"
#include "core/MetaProperty.h"
#include "core/Node.h"
#include "core/Types.h"

#include <gtest/gtest.h>

class VariantIndex_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::Variant::Index>>
{

};

TEST_P(VariantIndex_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::Variant::indexToString(value));
    EXPECT_EQ(value, dagbase::Variant::parseIndex(str));
}
//TYPE_INTEGER,
//TYPE_DOUBLE,
//TYPE_BOOL,
//TYPE_STRING,
//TYPE_COLOUR,
//TYPE_VEC2,
//TYPE_UINT

INSTANTIATE_TEST_SUITE_P(Variant, VariantIndex_testRoundTrip, ::testing::Values(
        std::make_tuple("TYPE_INTEGER", dagbase::Variant::TYPE_INTEGER),
        std::make_tuple("TYPE_DOUBLE", dagbase::Variant::TYPE_DOUBLE),
        std::make_tuple("TYPE_BOOL", dagbase::Variant::TYPE_BOOL),
        std::make_tuple("TYPE_STRING", dagbase::Variant::TYPE_STRING),
        std::make_tuple("TYPE_COLOUR", dagbase::Variant::TYPE_COLOUR),
        std::make_tuple("TYPE_VEC2", dagbase::Variant::TYPE_VEC2),
        std::make_tuple("TYPE_UINT", dagbase::Variant::TYPE_UINT),
        std::make_tuple("TYPE_FUNCTION", dagbase::Variant::TYPE_FUNCTION)
        ));

class ConfigurationElementRelOp_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(ConfigurationElementRelOp_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::ConfigurationElement::relOpToString(value));
    EXPECT_EQ(value, dagbase::ConfigurationElement::parseRelOp(str));
}

INSTANTIATE_TEST_SUITE_P(ConfigurationElement, ConfigurationElementRelOp_testRoundTrip, ::testing::Values(
        std::make_tuple("RELOP_UNKNOWN", dagbase::ConfigurationElement::RELOP_UNKNOWN),
        std::make_tuple("RELOP_EQ", dagbase::ConfigurationElement::RELOP_EQ),
        std::make_tuple("RELOP_NE", dagbase::ConfigurationElement::RELOP_NE),
        std::make_tuple("RELOP_LT", dagbase::ConfigurationElement::RELOP_LT),
        std::make_tuple("RELOP_LE", dagbase::ConfigurationElement::RELOP_LE),
        std::make_tuple("RELOP_GT", dagbase::ConfigurationElement::RELOP_GT),
        std::make_tuple("RELOP_GE", dagbase::ConfigurationElement::RELOP_GE)
        ));

class MetaPropertyFlags_testRoundTrip : public ::testing::TestWithParam<std::tuple<std::string, dagbase::MetaProperty::Flags>>
{

};

TEST_P(MetaPropertyFlags_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    EXPECT_EQ(str, dagbase::MetaProperty::flagsToString(value));
    EXPECT_EQ(value, dagbase::MetaProperty::parseFlags(str));
}

INSTANTIATE_TEST_SUITE_P(MetaPropertyFlags, MetaPropertyFlags_testRoundTrip, ::testing::Values(
    std::make_tuple("FLAGS_NONE", dagbase::MetaProperty::FLAGS_NONE),
    std::make_tuple("FLAGS_READ_BIT", dagbase::MetaProperty::FLAGS_READ_BIT),
    std::make_tuple("FLAGS_WRITE_BIT", dagbase::MetaProperty::FLAGS_WRITE_BIT),
    std::make_tuple("FLAGS_READ_BIT FLAGS_WRITE_BIT", static_cast<dagbase::MetaProperty::Flags>(dagbase::MetaProperty::FLAGS_READ_BIT | dagbase::MetaProperty::FLAGS_WRITE_BIT))
    ));

class NodeFlags_testRoundTrip : public ::testing::TestWithParam<std::tuple<std::string, dagbase::Node::NodeFlags>>
{

};

TEST_P(NodeFlags_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_EQ(str, dagbase::Node::flagsToString(value));
    EXPECT_EQ(value, dagbase::Node::parseFlags(str));
}

INSTANTIATE_TEST_SUITE_P(Node, NodeFlags_testRoundTrip, ::testing::Values(
    std::make_tuple("NODE_NONE", dagbase::Node::NODE_NONE),
    std::make_tuple("NODE_INPUT_BIT", dagbase::Node::NODE_INPUT_BIT),
    std::make_tuple("NODE_OUTPUT_BIT", dagbase::Node::NODE_OUTPUT_BIT),
    std::make_tuple("NODE_INTERNAL_BIT", dagbase::Node::NODE_INTERNAL_BIT)
));

class PortType_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::PortType::Type, const char*>>
{

};

TEST_P(PortType_testRoundTrip, testRoundTrip)
{
    auto typeStr = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());
    auto classStr = std::get<2>(GetParam());

    EXPECT_STREQ(typeStr, dagbase::PortType::toString(value));
    EXPECT_STREQ(classStr, dagbase::PortType::className(value));
    EXPECT_EQ(value, dagbase::PortType::parseFromString(typeStr));
}

INSTANTIATE_TEST_SUITE_P(PortType, PortType_testRoundTrip, ::testing::Values(
    std::make_tuple("TYPE_UINT8", dagbase::PortType::TYPE_UINT8, "TypedPort<uint8>"),
    std::make_tuple("TYPE_INT8", dagbase::PortType::TYPE_INT8, "TypedPort<int8>"),
    std::make_tuple("TYPE_UINT16", dagbase::PortType::TYPE_UINT16, "TypedPort<uint16>"),
    std::make_tuple("TYPE_INT16", dagbase::PortType::TYPE_INT16, "TypedPort<int16>"),
    std::make_tuple("TYPE_UINT32", dagbase::PortType::TYPE_UINT32, "TypedPort<uint32>"),
    std::make_tuple("TYPE_INT32", dagbase::PortType::TYPE_INT32, "TypedPort<int32>"),
    std::make_tuple("TYPE_UINT64", dagbase::PortType::TYPE_UINT64, "TypedPort<uint64>"),
    std::make_tuple("TYPE_INT64", dagbase::PortType::TYPE_INT64, "TypedPort<int64>"),
    std::make_tuple("TYPE_FLOAT", dagbase::PortType::TYPE_FLOAT, "TypedPort<float>"),
    std::make_tuple("TYPE_DOUBLE", dagbase::PortType::TYPE_DOUBLE, "TypedPort<double>"),
    std::make_tuple("TYPE_STRING", dagbase::PortType::TYPE_STRING, "TypedPort<string>"),
    std::make_tuple("TYPE_BOOL", dagbase::PortType::TYPE_BOOL, "TypedPort<bool>"),
    std::make_tuple("TYPE_OPAQUE", dagbase::PortType::TYPE_OPAQUE, "TypedPort<void*>")
));

class StatusCode_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::Status::StatusCode>>
{

};

TEST_P(StatusCode_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::Status::statusCodeToString(value));
    EXPECT_EQ(value, dagbase::Status::parseStatusCode(str));
}

INSTANTIATE_TEST_SUITE_P(Status, StatusCode_testRoundTrip, ::testing::Values(
    std::make_tuple("STATUS_OK", dagbase::Status::STATUS_OK),
    std::make_tuple("STATUS_FILE_NOT_FOUND", dagbase::Status::STATUS_FILE_NOT_FOUND),
    std::make_tuple("STATUS_OBJECT_NOT_FOUND", dagbase::Status::STATUS_OBJECT_NOT_FOUND),
    std::make_tuple("STATUS_INVALID_PORT", dagbase::Status::STATUS_INVALID_PORT),
    std::make_tuple("STATUS_INVALID_SELECTION", dagbase::Status::STATUS_INVALID_SELECTION),
    std::make_tuple("STATUS_CYCLE_DETECTED", dagbase::Status::STATUS_CYCLE_DETECTED),
    std::make_tuple("STATUS_SYNTAX_ERROR", dagbase::Status::STATUS_SYNTAX_ERROR),
    std::make_tuple("STATUS_FAILED_TO_CREATE_GRAPH", dagbase::Status::STATUS_FAILED_TO_CREATE_GRAPH),
    std::make_tuple("STATUS_UNKNOWN", dagbase::Status::STATUS_UNKNOWN)
));

class ResultType_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::Status::ResultType>>
{

};

TEST_P(ResultType_testRoundTrip, testRoundTrip)
{
    auto str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_STREQ(str, dagbase::Status::resultTypeToString(value));
    EXPECT_EQ(value, dagbase::Status::parseResultType(str));
}

INSTANTIATE_TEST_SUITE_P(Status, ResultType_testRoundTrip, ::testing::Values(
    std::make_tuple("RESULT_NONE", dagbase::Status::RESULT_NONE),
    std::make_tuple("RESULT_NODE", dagbase::Status::RESULT_NODE),
    std::make_tuple("RESULT_PORT", dagbase::Status::RESULT_PORT),
    std::make_tuple("RESULT_GRAPH", dagbase::Status::RESULT_GRAPH),
    std::make_tuple("RESULT_NODE_ID", dagbase::Status::RESULT_NODE_ID),
    std::make_tuple("RESULT_PORT_ID", dagbase::Status::RESULT_PORT_ID),
    std::make_tuple("RESULT_SIGNAL_PATH_ID", dagbase::Status::RESULT_SIGNAL_PATH_ID)
));

class PortFlags_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::Port::PortFlags>>
{

};

TEST_P(PortFlags_testRoundTrip, testRoundTrip)
{
    std::string str = std::get<0>(GetParam());
    auto value = std::get<1>(GetParam());

    EXPECT_EQ(str, dagbase::Port::portFlagsToString(value));
    EXPECT_EQ(value, dagbase::Port::parsePortFlags(str));
}

INSTANTIATE_TEST_SUITE_P(PortFlags, PortFlags_testRoundTrip, ::testing::Values(
    std::make_tuple("FLAGS_NONE", dagbase::Port::FLAGS_NONE),
    std::make_tuple("OWN_META_PORT_BIT", dagbase::Port::OWN_META_PORT_BIT),
    std::make_tuple("OWN_INPUTS_BIT", dagbase::Port::OWN_INPUTS_BIT),
    std::make_tuple("OWN_OUTPUTS_BIT", dagbase::Port::OWN_OUTPUTS_BIT),
    std::make_tuple("REMOVED_BIT", dagbase::Port::REMOVED_BIT),
    std::make_tuple("OWN_INPUTS_BIT OWN_OUTPUTS_BIT", static_cast<dagbase::Port::PortFlags>(dagbase::Port::OWN_INPUTS_BIT | dagbase::Port::OWN_OUTPUTS_BIT))
    ));

class NodeCategory_testRoundTrip : public ::testing::TestWithParam<std::tuple<const char*, dagbase::NodeCategory::Category>>
{

};

TEST_P(NodeCategory_testRoundTrip, testRoundTrip)
{
    const char* categoryString = std::get<0>(GetParam());
    dagbase::NodeCategory::Category category = std::get<1>(GetParam());
    EXPECT_STREQ(categoryString, dagbase::NodeCategory::toString(category));
    EXPECT_EQ(category, dagbase::NodeCategory::parse(categoryString));
}

INSTANTIATE_TEST_SUITE_P(NodeCategory, NodeCategory_testRoundTrip, ::testing::Values(
    std::make_tuple("None",dagbase::NodeCategory::CAT_NONE),
    std::make_tuple("Source",dagbase::NodeCategory::CAT_SOURCE),
    std::make_tuple("Sink",dagbase::NodeCategory::CAT_SINK),
    std::make_tuple("Condition",dagbase::NodeCategory::CAT_CONDITION),
    std::make_tuple("Action",dagbase::NodeCategory::CAT_ACTION),
    std::make_tuple("Group",dagbase::NodeCategory::CAT_GROUP),
    std::make_tuple("Unknown",dagbase::NodeCategory::CAT_UNKNOWN)
));
