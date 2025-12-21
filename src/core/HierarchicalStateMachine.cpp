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
            // if (auto itAction=_exitActions.m.find(_currentState.name); itAction != _exitActions.m.end())
            // {
            //     itAction->second(_currentState);
            // }
            auto nextState = parseState(it->second.nextState);
            // if (auto itAction=_transitionActions.m.find(std::make_pair(_currentState.name, it->second.nextState)); itAction!=_transitionActions.m.end())
            // {
            //     itAction->second(_currentState);
            // }
            _currentState = nextState;
            // if (auto itAction=_entryActions.m.find(_currentState.name); itAction != _entryActions.m.end())
            // {
            //     itAction->second(_currentState);
            // }
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
}
