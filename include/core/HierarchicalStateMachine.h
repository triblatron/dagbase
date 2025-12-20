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

    class DAGBASE_API HierarchicalInput
    {
    public:
        void configure(ConfigurationElement& config);

        std::uint32_t value() const
        {
            return _value;
        }

        Variant find(std::string_view path) const;
    private:
        std::uint32_t _value{0};
    };

    class DAGBASE_API HierarchicalStateMachine
    {
    public:
        enum Flags : std::uint32_t
        {
            FLAGS_NONE,
            //! We are a simple state rather than a state machine.
            FLAGS_HAS_VALUE
        };
    public:
        HierarchicalStateMachine() = default;

        ~HierarchicalStateMachine();

        void configure(ConfigurationElement& config);

        std::int64_t value() const
        {
            return _value;
        }

        bool isFlagSet(Flags mask) const
        {
            return (_flags & mask) != 0;
        }

        Variant find(std::string_view path) const;
    private:
        void setFlag(Flags mask)
        {
            _flags = static_cast<Flags>(_flags | mask);
        }
        using ChildArray = SearchableMapFromAtom<VectorMap<Atom, HierarchicalStateMachine *>>;
        ChildArray _children;
        std::int64_t _value{0};
        SearchableMapFromAtom<VectorMap<Atom,HierarchicalInput>> _inputs;
        Atom _initialState;
        HierarchicalStateMachine* _currentState{nullptr};
        Flags _flags{FLAGS_NONE};
        HierarchicalStateMachine* findInitialState();
    };
}
