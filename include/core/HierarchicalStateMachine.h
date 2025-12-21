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
    class HierarchicalStateMachine;

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

    struct DAGBASE_API HierarchicalTransition
    {
        Atom nextState;

        struct Domain
        {
            Atom initialState;
            Atom input;

            bool operator<(const Domain& other) const
            {
                return initialState < other.initialState || (initialState == other.initialState && input<other.input);
            }

            bool operator==(const Domain& other) const
            {
                return initialState == other.initialState && input == other.input;
            }

            void configure(ConfigurationElement& config)
            {
                ConfigurationElement::readConfig(config, "initialState", &initialState);
                ConfigurationElement::readConfig(config, "input", &input);
            }
        };

        struct Codomain
        {
            Atom nextState;

            void configure(ConfigurationElement& config)
            {
                ConfigurationElement::readConfig(config, "nextState", &nextState);
            }
        };
    };

    struct HierarchicalEntryExitAction
    {
        void configure(ConfigurationElement& config)
        {
            _func = config.asFunction();
        }

        Variant find(std::string_view path) const
        {
            Variant retval;

            retval = findEndpoint(path, "numCalls", numCalls);
            if (retval.has_value())
                return retval;

            return {};
        }

        void operator()(HierarchicalStateMachine& state);

        std::int64_t     numCalls{0};
        dagbase::Function* _func{nullptr};
    };

    class DAGBASE_API HierarchicalStateMachine
    {
    public:
        enum Flags : std::uint32_t
        {
            FLAGS_NONE,
            //! We are a simple state rather than a state machine.
            FLAGS_HAS_VALUE = 1<<0,
            // This is a final state.
            FLAGS_FINAL = 1<<1
        };
        using ChildArray = SearchableMapFromAtom<VectorMap<Atom, HierarchicalStateMachine *>>;
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

        ChildArray::iterator state()
        {
            return _currentState;
        }

        ChildArray::iterator parseState(const Atom & atom);

        void onInput(const Atom& input);

        Variant find(std::string_view path) const;
    private:
        void setFlag(Flags mask)
        {
            _flags = static_cast<Flags>(_flags | mask);
        }
        ChildArray _children;
        std::int64_t _value{0};
        SearchableMapFromAtom<VectorMap<Atom,HierarchicalInput>> _inputs;
        Atom _initialState;
        ChildArray::iterator _currentState{_children.end()};
        Flags _flags{FLAGS_NONE};
        ChildArray::iterator findInitialState();
        using TransitionFunction=VectorMap<HierarchicalTransition::Domain,HierarchicalTransition::Codomain>;
        TransitionFunction _transitionFunction;
        using EntryExitActions = SearchableMapFromAtom<VectorMap<Atom,HierarchicalEntryExitAction>>;
        EntryExitActions _entryActions;
        EntryExitActions _exitActions;
        void readEntryExitActions(ConfigurationElement& config, const char* name, EntryExitActions* value);
        using TransitionActions = SearchableMapFromAtomPair<VectorMap<std::pair<Atom,Atom>,HierarchicalEntryExitAction>>;
        TransitionActions _transitionActions;
        void readTransitionActions(ConfigurationElement& config, const char* name, TransitionActions* value);
    };
}
