//
// Created by Tony Horrobin on 27/09/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "util/SearchableArray.h"
#include "core/Variant.h"

#include <variant>
#include <cstdint>
#include <string>
#include <string_view>

namespace dagbase
{
    class DAGBASE_API VariantArray
    {
    public:
        //using ValueType = std::optional<std::variant<std::int64_t, double, bool, std::string, Colour, Vec2, std::uint32_t>>;
        using ArrayOfInt64 = SearchablePrimitiveArray<std::vector<std::int64_t>>;
        using ArrayOfDouble = SearchablePrimitiveArray<std::vector<double>>;
        using ArrayOfBool = SearchablePrimitiveArray<std::vector<bool>>;
        using ArrayOfString = SearchablePrimitiveArray<std::vector<std::string>>;
        using ArrayOfColour = SearchablePrimitiveArray<std::vector<Colour>>;
        using ArrayOfVec2 = SearchablePrimitiveArray<std::vector<Vec2>>;
        using ArrayOfUInt32 = SearchablePrimitiveArray<std::vector<std::uint32_t>, std::int64_t>;
        using ValueType = std::variant<ArrayOfInt64, ArrayOfDouble, ArrayOfBool, ArrayOfString, ArrayOfColour, ArrayOfVec2, ArrayOfUInt32>;
    public:
        void configure(ConfigurationElement& config);

        Variant find(std::string_view path) const;
    private:
        ValueType _value;
    };
}