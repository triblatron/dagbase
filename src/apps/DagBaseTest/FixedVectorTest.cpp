//
// Created by Tony Horrobin on 28/12/2025.
//

#include "util/FixedVector.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"

#include <gtest/gtest.h>

using TestFixedVector = dagbase::FixedVector<int, 10>;

class FixedVector_testEmplaceBack : public ::testing::TestWithParam<std::tuple<const char*>>
{
public:
    struct Emplacement
    {
        int value{0};

        void configure(dagbase::ConfigurationElement& config);

        void makeItSo(TestFixedVector& sut);
    };
public:
    void configure(dagbase::ConfigurationElement& config);

    void makeItSo(TestFixedVector& sut);
private:
    std::uint32_t _expectedSize{0};
    std::vector<Emplacement> _emplacements;
};

void FixedVector_testEmplaceBack::Emplacement::configure(dagbase::ConfigurationElement &config)
{
    dagbase::ConfigurationElement::readConfig(config, "value", &value);
}

void FixedVector_testEmplaceBack::Emplacement::makeItSo(TestFixedVector &sut)
{
    sut.emplace_back(std::forward<int>(value));
}

void FixedVector_testEmplaceBack::configure(dagbase::ConfigurationElement& config)
{
    dagbase::ConfigurationElement::readConfig(config, "size", &_expectedSize);
    if (auto element=config.findElement("emplacements"); element)
    {
        element->eachChild([this](dagbase::ConfigurationElement& child) {
            Emplacement entry;

            entry.configure(child);
            _emplacements.emplace_back(entry);

            return true;
        });
    }
}

void FixedVector_testEmplaceBack::makeItSo(TestFixedVector& sut)
{
    for (auto& entry : _emplacements)
    {
        entry.makeItSo(sut);
    }
    ASSERT_EQ(_expectedSize, sut.size());
}

TEST_P(FixedVector_testEmplaceBack, testExpectedSize)
{
    auto configStr = std::get<0>(GetParam());
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, configStr);
    ASSERT_NE(nullptr, config);
    configure(*config);
    dagbase::FixedVector<int, 10> sut;
    makeItSo(sut);;
}

INSTANTIATE_TEST_SUITE_P(FixedVector, FixedVector_testEmplaceBack, ::testing::Values(
    std::make_tuple("data/tests/FixedVector/EmplaceOne.lua"),
    std::make_tuple("data/tests/FixedVector/EmplaceMany.lua"),
    std::make_tuple("data/tests/FixedVector/EmplaceMax.lua"),
    std::make_tuple("data/tests/FixedVector/EmplaceTooMany.lua")
    ));