//
// Created by Tony Horrobin on 18/12/2025.
//

#pragma once

#include "config/DagBaseExport.h"
#include "core/Variant.h"
#include "util/VectorMap.h"
#include "core/Atom.h"
#include "util/SearchableMap.h"

#include <string_view>

namespace dagbase
{
    class ConfigurationElement;

    class DAGBASE_API SimpleState
    {
    public:
        SimpleState() = default;

        virtual ~SimpleState() = default;

        virtual void configure(ConfigurationElement& config);

        virtual Variant find(std::string_view path) const;
    private:
        std::int64_t _value{0};
    };

    class DAGBASE_API HierarchicalStateMachine : public SimpleState
    {
    public:
        ~HierarchicalStateMachine() override;

        void configure(ConfigurationElement& config) override;

        Variant find(std::string_view path) const override;
    private:
        SearchableMapFromAtom<VectorMap<Atom,SimpleState*>> _states;
    };
}
