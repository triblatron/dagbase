//
// Created by Tony Horrobin on 29/06/2026.
//

#include "config/config.h"

#include "core/Types.h"
#include "core/LuaInterface.h"
#include "test/TestUtils.h"

#include <gtest/gtest.h>


class Status_testConfigure : public ::testing::TestWithParam<std::tuple<const char*, const char*, dagbase::Variant, double, dagbase::ConfigurationElement::RelOp>>
{

};

TEST_P(Status_testConfigure, testExpectedValue)
{
    auto configFilename = std::get<0>(GetParam());
    auto path = std::get<1>(GetParam());
    auto value = std::get<2>(GetParam());
    auto tolerance = std::get<3>(GetParam());
    auto op = std::get<4>(GetParam());

    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configFilename);
    ASSERT_NE(nullptr, config);
    dagbase::Status sut;
    sut.configure(*config);
    auto actual = sut.find(path);
    assertComparison(value, actual, tolerance, op, path);
}

INSTANTIATE_TEST_SUITE_P(Status, Status_testConfigure, ::testing::Values(
    std::make_tuple("data/tests/Status/None.lua", "statusCode", dagbase::Variant(std::uint32_t{dagbase::Status::STATUS_OK}), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/Status/None.lua", "resultType", dagbase::Variant(std::uint32_t{dagbase::Status::RESULT_NONE}), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/Status/None.lua", "nodeID", dagbase::Variant(), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/Status/None.lua", "PortID", dagbase::Variant(), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/Status/None.lua", "SignalPathID", dagbase::Variant(), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/Status/NodeID.lua", "nodeID", dagbase::NodeID(0), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/Status/PortID.lua", "portID", dagbase::PortID(0), 0.0, dagbase::ConfigurationElement::RELOP_EQ),
    std::make_tuple("data/tests/Status/SignalPathID.lua", "signalPathID", dagbase::PortID(0), 0.0, dagbase::ConfigurationElement::RELOP_EQ)
    ));
