//
// Created by Tony Horrobin on 18/12/2025.
//

#pragma once

#include "config/DagBaseExport.h"
#include "core/Variant.h"
#include "util/VectorMap.h"
#include "core/Atom.h"
#include "util/SearchableMap.h"
#include "util/SearchableArray.h"

#include <string_view>

namespace dagbase
{
    class ConfigurationElement;

    class DAGBASE_API HierarchicalState
    {
    public:
        HierarchicalState() = default;

        virtual ~HierarchicalState();

        virtual void configure(ConfigurationElement& config);

        virtual Variant find(std::string_view path) const;
    private:
        using ChildArray = SearchableArray<std::vector<HierarchicalState*>>;
        ChildArray _children;
        std::int64_t _value{0};
    };

    class DAGBASE_API HierarchicalStateMachine
    {
    public:
        ~HierarchicalStateMachine();

        void configure(ConfigurationElement& config);

        Variant find(std::string_view path) const;
    private:
        SearchableMapFromAtom<VectorMap<Atom,HierarchicalState*>> _states;
        HierarchicalState* _currentState{nullptr};
    };
}
