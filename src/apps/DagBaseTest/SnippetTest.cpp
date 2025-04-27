//
// Created by Tony Horrobin on 25/04/2025.
//

#include "core/Snippet.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"
#include "core/Snippet.h"
#include "util/VectorMap.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class FakeSnippet : public dagbase::Snippet
{
public:
    FakeSnippet()
    {
        ON_CALL(*this, resolve(::testing::_)).WillByDefault([this](const std::string& name) {
            if (auto it=_lookup.find(name); it!=_lookup.end())
            {
                std::ostringstream str;
                str << it->second.value();
                return str.str();
            }
            return std::string();
        });
    }
    void configure(dagbase::ConfigurationElement& config) override
    {
        dagbase::Snippet::configure(config);
        if (auto element=config.findElement("variables"); element)
        {
            element->eachChild([this](dagbase::ConfigurationElement& child) {
                if (child.numChildren()==2)
                    _lookup.insert(VariableLookup::value_type(child.child(0)->asString(), child.child(1)->value()));
                return true;
            });
        }
    }
    MOCK_METHOD(std::string, resolve, (const std::string&), (override));
private:
    using VariableLookup = dagbase::VectorMap<std::string, dagbase::ConfigurationElement::ValueType>;
    VariableLookup _lookup;
};

class Snippet_testInterpolate : public ::testing::TestWithParam<std::tuple<const char*, const char*, int>>
{

};

TEST_P(Snippet_testInterpolate, testExpectedOutput)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    FakeSnippet sut;
    sut.configure(*config);
    auto output = std::get<1>(GetParam());
    auto numCalls = std::get<2>(GetParam());
    EXPECT_CALL(sut, resolve(::testing::_)).Times(numCalls);

    auto actualOutput = sut.interpolate();
    EXPECT_EQ(output, actualOutput);
}

INSTANTIATE_TEST_SUITE_P(Snippet, Snippet_testInterpolate, ::testing::Values(
        std::make_tuple("data/tests/Snippet/NoVariables.lua", "foo=true", 0),
        std::make_tuple("data/tests/Snippet/OneVariable.lua", "foo=true", 1),
        std::make_tuple("data/tests/Snippet/MultipleVariables.lua", "foo=true\nbar=1.5\n", 2)
        ));