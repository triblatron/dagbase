//
// Created by Tony Horrobin on 10/09/2025.
//

#pragma once

#include "core/ConfigurationElement.h"
#include "core/Variant.h"
#include "util/Searchable.h"
#include "util/VectorSet.h"
#include "util/VectorMap.h"
#include "util/SearchableMap.h"

#include <cstdint>

namespace dagbase
{
    template<typename State, typename Transition, typename Input, typename EntryExitAction>
    class StateMachine
    {
     public:
        void configure(dagbase::ConfigurationElement& config)
        {
            dagbase::ConfigurationElement::readConfigSet(config, "states", &_states);

            dagbase::ConfigurationElement::readConfigSet(config, "inputs", &_inputs);

            dagbase::ConfigurationElement::readConfigVectorMap(config, "transitionFunction", &_transitionFunction);

            readEntryExitActions(config, "entryActions", &_entryActions);

            readEntryExitActions(config, "exitActions", &_exitActions);

            readTransitionActions(config, "transitionActions", &_transitionActions);

            dagbase::ConfigurationElement::readConfig(config, "initialState", &_initialState.name);

            _initialState = parseState(_initialState.name);
            _currentState = _initialState;
        }

        void onInput(typename Input::Name input)
        {
            typename Transition::Domain domain;
            domain.initialState = _currentState.name;
            domain.input = input;
            if (auto it= _transitionFunction.find(domain); it!=_transitionFunction.end())
            {
                if (auto itAction=_exitActions.m.find(_currentState.name); itAction != _exitActions.m.end())
                {
                    itAction->second(_currentState);
                }
                auto nextState = parseState(it->second.nextState);
                if (auto itAction=_transitionActions.m.find(std::make_pair(_currentState.name, it->second.nextState)); itAction!=_transitionActions.m.end())
                {
                    itAction->second(_currentState);
                }
                _currentState = nextState;
                if (auto itAction=_entryActions.m.find(_currentState.name); itAction != _entryActions.m.end())
                {
                    itAction->second(_currentState);
                }
            }
        }

        State state() const
        {
            return _currentState;
        }

        void step()
        {

        }

        dagbase::Variant find(std::string_view path) const
        {
            dagbase::Variant retval;

            retval = dagbase::findEndpoint(path, "numStates", std::uint32_t(_states.size()));
            if (retval.has_value())
                return retval;

            retval = dagbase::findEndpoint(path, "numInputs", std::uint32_t(_inputs.size()));
            if (retval.has_value())
                return retval;

            retval = dagbase::findEndpoint(path, "numTransitions", std::uint32_t(_transitionFunction.size()));
            if (retval.has_value())
                return retval;

            retval = dagbase::findEndpoint(path, "numEntryActions", std::uint32_t(_entryActions.size()));
            if (retval.has_value())
                return retval;

            retval = dagbase::findEndpoint(path, "numExitActions", std::uint32_t(_exitActions.size()));
            if (retval.has_value())
                return retval;

            retval = dagbase::findInternal(path, "entryActions", _entryActions);
            if (retval.has_value())
                return retval;

            retval = dagbase::findInternal(path, "exitActions", _exitActions);
            if (retval.has_value())
                return retval;

            retval = dagbase::findInternal(path, "transitionActions", _transitionActions);
            if (retval.has_value())
                return retval;

            return {};
        }

        bool accepted() const
        {
            return _currentState.final;
        }

        typename Input::Value parseInput(typename Input::Name name)
        {
            for (auto input : _inputs)
            {
                if (input.name == name)
                {
                    return input.value;
                }
            }

            return 0;
        }

        State parseState(typename State::Name name)
        {
            for (auto state : _states)
            {
                if (state.name == name)
                    return state;
            }

            return _initialState;
        }
    private:
        State _initialState;
        State _currentState;
        using StateSet = dagbase::VectorSet<State>;
        StateSet _states;
        using InputSet = dagbase::VectorSet<Input>;
        InputSet _inputs;

        using TransitionFunction = dagbase::VectorMap<typename Transition::Domain,typename Transition::Codomain>;
        TransitionFunction _transitionFunction;
        using EntryExitActions = dagbase::SearchableMapFromAtom<dagbase::VectorMap<dagbase::Atom,EntryExitAction>>;
        EntryExitActions _entryActions;
        EntryExitActions _exitActions;
        void readEntryExitActions(dagbase::ConfigurationElement& config, const char* name, EntryExitActions* value);
        using TransitionActions = dagbase::SearchableMapFromAtomPair<dagbase::VectorMap<std::pair<dagbase::Atom,dagbase::Atom>,EntryExitAction>>;
        TransitionActions _transitionActions;
        void readTransitionActions(dagbase::ConfigurationElement& config, const char* name, TransitionActions* value);
    };
    template<typename State, typename Transition, typename Input, typename EntryExitAction>
    void StateMachine<State, Transition, Input, EntryExitAction>::readTransitionActions(
            dagbase::ConfigurationElement &config, const char *name, StateMachine::TransitionActions *value)
    {
        if (value)
            if (auto element=config.findElement(name); element)
            {
                value->reserve(element->numChildren());
                element->eachChild([this](dagbase::ConfigurationElement& child) {
                    dagbase::Atom fromState;
                    dagbase::Atom toState;
                    dagbase::ConfigurationElement::readConfig(child, "fromState", &fromState);
                    dagbase::ConfigurationElement::readConfig(child, "toState", &toState);
                    EntryExitAction action;
                    if (auto actionElement=child.findElement("action"); actionElement)
                    {
                        action.configure(*actionElement);
                    }

                    _transitionActions.emplace(std::make_pair(fromState, toState), action);

                    return true;
                });
            }
    }

    
    template<typename State, typename Transition, typename Input, typename EntryExitAction>
    void
    StateMachine<State, Transition, Input, EntryExitAction>::readEntryExitActions(dagbase::ConfigurationElement &config,
                                                                                  const char *name,
                                                                                  StateMachine::EntryExitActions *value)
    {
        if (value)
            if (auto element=config.findElement(name); element)
            {
                value->reserve(element->numChildren());
                element->eachChild([&value](dagbase::ConfigurationElement& child) {
                    dagbase::Atom state;
                    EntryExitAction action;
                    dagbase::ConfigurationElement::readConfig(child, "state", &state);

                    if (auto actionElement=child.findElement("action"); actionElement)
                    {
                        action.configure(*actionElement);
                    }

                    value->emplace(state, action);

                    return true;
                });
            }
    }
}
