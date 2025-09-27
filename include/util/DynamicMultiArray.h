//
// Created by Tony Horrobin on 25/09/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Variant.h"
#include "util/Searchable.h"
#include "util/SearchableArray.h"
#include "core/VariantArray.h"

#include <vector>
#include <string_view>

namespace dagbase
{
    class ConfigurationElement;

    class DAGBASE_API DynamicMultiArray
    {
    public:
        void configure(ConfigurationElement& config);

        void emplace_back(const VariantArray & a);

        Variant find(std::string_view path) const;
    private:
        using Arrays = SearchableArray<std::vector<VariantArray>>;
        Arrays _arrays;
    };
}