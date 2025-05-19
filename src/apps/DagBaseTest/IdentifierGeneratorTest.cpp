//
// Created by Tony Horrobin on 06/05/2025.
//

#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"
#include "core/IdentifierGenerator.h"

#include <gtest/gtest.h>

class IdentifierGenerator_testGenerate : public ::testing::TestWithParam<std::tuple<const char*>>
{

};

TEST_P(IdentifierGenerator_testGenerate, testExpectedIdentifier)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    dagbase::IdentifierGenerator sut;
    config->eachChild([this,&sut](dagbase::ConfigurationElement& child) {
        std::string cmd;

        if (auto element=child.findElement("cmd"); element)
        {
            cmd = element->asString();
        }

        std::uint32_t count = 0;
        if (auto element=child.findElement("count"); element)
        {
            count = element->asInteger();
        }
        dagbase::IdentifierGenerator::Identifier expectedId;

        if (auto element=child.findElement("expectedId"); element)
        {
            expectedId = element->asInteger();
        }

        if (cmd == "generate")
        {
            dagbase::IdentifierGenerator::Identifier actualId = 0;
            for (int i=0; i<count; ++i)
                actualId = sut.generate();

            EXPECT_EQ(expectedId, actualId);
        }
        else if (cmd == "release")
        {
            sut.release(expectedId);
        }

        return true;
    });
}

INSTANTIATE_TEST_SUITE_P(IdentifierGenerator, IdentifierGenerator_testGenerate, ::testing::Values(
        std::make_tuple("data/tests/IdentifierGenerator/One.lua"),
        std::make_tuple("data/tests/IdentifierGenerator/Two.lua"),
        std::make_tuple("data/tests/IdentifierGenerator/ReuseOne.lua"),
        std::make_tuple("data/tests/IdentifierGenerator/RecycleOne.lua"),
        std::make_tuple("data/tests/IdentifierGenerator/Mixture.lua")
        ));