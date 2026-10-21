//
// Created by Tony Horrobin on 18/10/2026.
//

#include "config/config.h"
#include "gtest/gtest.h"

#include "util/VariableLengthInteger.h"
#include "core/LuaInterface.h"
#include "core/ConfigurationElement.h"

#include <memory_resource>
#include <vector>

#include "util/CharConv.h"

template<typename T>
struct VariableLengthIntegerCase
{
    void configure(dagbase::ConfigurationElement& config)
    {
        dagbase::ConfigurationElement::readConfig<dagbase::Value::Type>(config, "valueType", &dagbase::Value::parseType, &valueType);
        value = dagbase::Value(valueType);
        dagbase::ConfigurationElement::readConfig(config, "value", true, &value);
        dagbase::ConfigurationElement::readConfig(config, "size", &size);
    }

    void makeItSo() const
    {
        dagbase::VariableLengthInteger<T> sut{std::get<T>(value.value())};
        std::array<std::byte, 10> buf{};
        std::pmr::monotonic_buffer_resource pool(&buf, sizeof(buf));
        std::pmr::vector<std::uint8_t> output;
        sut.encode(&output);
        dagbase::VariableLengthInteger<T> actual{0};
        actual.decode(output);
        EXPECT_EQ(sut.value, actual.value);
        EXPECT_EQ(size, output.size()) << "Expected size of " << size << " for " << value;
    }
    dagbase::Value::Type valueType{dagbase::Value::TYPE_UNKNOWN};
    dagbase::Value value;
    std::uint32_t size{1};
};

template<typename T>
struct VariableLengthIntegerType
{
    void configure(dagbase::ConfigurationElement& config)
    {
        config.eachChild([this](dagbase::ConfigurationElement& child) {
            VariableLengthIntegerCase<T> item;

            item.configure(child);
            cases.emplace_back(item);

            return true;
        });
    }

    void makeItSo() const
    {
        for (auto it=cases.begin(); it!=cases.end(); ++it)
        {
            it->makeItSo();
        }
    }

    using CaseArray = std::vector<VariableLengthIntegerCase<T>>;
    CaseArray cases;
};
template<typename T>
class VariableLengthInteger_testRoundTrip : public ::testing::Test
{
public:
    void configure(dagbase::ConfigurationElement& config)
    {
        config.eachChild([this](dagbase::ConfigurationElement& child) {
            auto typeName = child.name();
            VariableLengthIntegerType<T> type;

            type.configure(child);
            _types.emplace(typeName, type);

            return true;
        });
    }

    void makeItSo(const VariableLengthIntegerType<T>& type)
    {
        type.makeItSo();
    }
protected:
    T value{};
    using TypeMap=std::map<std::string, VariableLengthIntegerType<T>>;
    TypeMap _types;
};

using IntegerTypes = ::testing::Types<std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t, std::int32_t, std::uint64_t, std::int64_t>;
TYPED_TEST_SUITE(VariableLengthInteger_testRoundTrip, IntegerTypes);

TYPED_TEST(VariableLengthInteger_testRoundTrip, testRoundTrip)
{
    dagbase::Lua lua;
    auto config = dagbase::ConfigurationElement::fromFile(lua, "data/tests/VariableLengthInteger/test.lua");
    ASSERT_NE(nullptr, config);
    if (auto element = config->findElement("cases"); element)
    {
        element->eachChild([this](dagbase::ConfigurationElement& child) {
            auto typeName = child.name();
            VariableLengthIntegerType<TypeParam> item;

            item.configure(child);

            this->_types.emplace(typeName, item);

            return true;
        });
    }

    if constexpr (sizeof(TypeParam) == 1 && std::is_unsigned_v<TypeParam>)
    {
        if (auto it=this->_types.find("TYPE_UINT8"); it!=this->_types.end())
        {
            this->makeItSo(it->second);
        }
    }
    else if constexpr (sizeof(TypeParam) == 1 && std::is_signed_v<TypeParam>)
    {
        if (auto it=this->_types.find("TYPE_INT8"); it!=this->_types.end())
        {
            this->makeItSo(it->second);
        }
    }
    else if constexpr (sizeof(TypeParam) == 2 && std::is_unsigned_v<TypeParam>)
    {
        if (auto it=this->_types.find("TYPE_UINT16"); it!=this->_types.end())
        {
            this->makeItSo(it->second);
        }
    }
    else if constexpr (sizeof(TypeParam) == 2 && std::is_signed_v<TypeParam>)
    {
        if (auto it=this->_types.find("TYPE_INT16"); it!=this->_types.end())
        {
            this->makeItSo(it->second);
        }
    }
    else if constexpr (sizeof(TypeParam) == 4 && std::is_unsigned_v<TypeParam>)
    {
        if (auto it=this->_types.find("TYPE_UINT32"); it!=this->_types.end())
        {
            this->makeItSo(it->second);
        }
    }
    else if constexpr (sizeof(TypeParam) == 4 && std::is_signed_v<TypeParam>)
    {
        if (auto it=this->_types.find("TYPE_INT32"); it!=this->_types.end())
        {
            this->makeItSo(it->second);
        }
    }
    else if constexpr (sizeof(TypeParam) == 8 && std::is_unsigned_v<TypeParam>)
    {
        if (auto it=this->_types.find("TYPE_UINT64"); it!=this->_types.end())
        {
            this->makeItSo(it->second);
        }
    }
}
