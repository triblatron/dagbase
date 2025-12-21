//
// Created by Tony Horrobin on 18/12/2025.
//

#include "config/config.h"

#include "core/HierarchicalStateMachine.h"
#include "core/ConfigurationElement.h"
#include "util/Searchable.h"

namespace dagbase
{
    void HierarchicalStateMachine::configure(ConfigurationElement& config)
    {
        if (auto element=config.findElement("states"); element)
        {
            element->eachChild([this](ConfigurationElement& child) {
                HierarchicalStateMachine* state = nullptr;
                state = new HierarchicalStateMachine();
                state->configure(child);
                Atom name;
                ConfigurationElement::readConfig(child, "name", &name);
                _children.emplace(name, state);
                return true;
            });
        }

        bool final = false;
        ConfigurationElement::readConfig(config, "final", &final);
        if (final)
            setFlag(FLAGS_FINAL);

        if (ConfigurationElement::readConfig(config, "value", &_value))
        {
            setFlag(FLAGS_HAS_VALUE);
        }

        ConfigurationElement::readConfig(config, "initialState", &_initialState);
        _currentState = findInitialState();
        if (auto element=config.findElement("inputs"); element)
        {
            element->eachChild([this](ConfigurationElement& child) {
                HierarchicalInput input;

                Atom name;
                ConfigurationElement::readConfig(child, "name", &name);
                input.configure(child);
                _inputs.emplace(name, input);

                return true;
            });
        }
        ConfigurationElement::readConfigVectorMap(config, "transitionFunction", &_transitionFunction);
        readEntryExitActions(config, "entryActions", &_entryActions);

        readEntryExitActions(config, "exitActions", &_exitActions);

        readTransitionActions(config, "transitionActions", &_transitionActions);

    }

    HierarchicalStateMachine::ChildArray::iterator HierarchicalStateMachine::parseState(const Atom &name)
    {
        return _children.m.find(name);
    }

    void HierarchicalStateMachine::onInput(const Atom& input)
    {
        HierarchicalTransition::Domain domain;
        domain.initialState = _currentState->first;
        domain.input = input;
        if (auto it= _transitionFunction.find(domain); it!=_transitionFunction.end())
        {
            if (auto itAction=_exitActions.m.find(_currentState->first); itAction != _exitActions.m.end())
            {
                itAction->second(*_currentState->second);
            }
            auto nextState = parseState(it->second.nextState);
            if (auto itAction=_transitionActions.m.find(std::make_pair(_currentState->first, it->second.nextState)); itAction!=_transitionActions.m.end())
            {
                itAction->second(*_currentState->second);
            }
            _currentState = nextState;
            if (auto itAction=_entryActions.m.find(_currentState->first); itAction != _entryActions.m.end())
            {
                itAction->second(*_currentState->second);
            }
        }
    }

    Variant HierarchicalStateMachine::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "numStates", std::uint32_t(_children.m.size()));
        if (retval.has_value())
            return retval;

        if (_currentState!=_children.end())
        {
            retval = findEndpoint(path, "currentState", _currentState->second->value());
            if (retval.has_value())
                return retval;
        }

        retval = findEndpoint(path, "numInputs", std::uint32_t(_inputs.size()));
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "numTransitions", std::uint32_t(_transitionFunction.size()));
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "states", _children);
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "inputs", _inputs);
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "entryActions", _entryActions);
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "exitActions", _exitActions);
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "transitionActions", _transitionActions);
        if (retval.has_value())
            return retval;

        return {};
    }

    HierarchicalStateMachine::ChildArray::iterator HierarchicalStateMachine::findInitialState()
    {
        if (auto it=_children.m.find(_initialState); it!=_children.m.end())
        {
            if (it->second->isFlagSet(FLAGS_HAS_VALUE))
            {
                return it;
            }
            else
            {
                return it->second->findInitialState();
            }
        }
        return _children.end();
    }

    void HierarchicalEntryExitAction::operator()(HierarchicalStateMachine &state)
    {
        (func)(state);
        ++numCalls;
    }

    HierarchicalStateMachine::~HierarchicalStateMachine()
    {
        for (auto& p : _children)
        {
            delete p.second;
        }
    }

    void HierarchicalInput::configure(ConfigurationElement &config)
    {
        ConfigurationElement::readConfig(config, "value", &_value);
    }

    Variant HierarchicalInput::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "value", _value);
        if (retval.has_value())
            return retval;

        return {};
    }

    void HierarchicalStateMachine::readTransitionActions(
            dagbase::ConfigurationElement &config, const char *name, HierarchicalStateMachine::TransitionActions *value)
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
                    HierarchicalEntryExitAction action;
                    if (auto actionElement=child.findElement("action"); actionElement)
                    {
                        action.configure(*actionElement);
                    }

                    _transitionActions.emplace(std::make_pair(fromState, toState), action);

                    return true;
                });
            }
    }


    void
    HierarchicalStateMachine::readEntryExitActions(dagbase::ConfigurationElement &config,
                                                                                  const char *name,
                                                                                  EntryExitActions *value)
    {
        if (value)
            if (auto element=config.findElement(name); element)
            {
                value->reserve(element->numChildren());
                element->eachChild([&value](dagbase::ConfigurationElement& child) {
                    dagbase::Atom state;
                    HierarchicalEntryExitAction action;
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
