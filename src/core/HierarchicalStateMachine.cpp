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

    Variant HierarchicalStateMachine::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "numStates", std::uint32_t(_children.m.size()));
        if (retval.has_value())
            return retval;

        if (_currentState)
        {
            retval = findEndpoint(path, "currentState", _currentState->value());
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

    HierarchicalStateMachine * HierarchicalStateMachine::findInitialState()
    {
        if (auto it=_children.m.find(_initialState); it!=_children.m.end())
        {
            if (it->second->isFlagSet(FLAGS_HAS_VALUE))
            {
                return it->second;
            }
            else
            {
                return it->second->findInitialState();
            }
        }
        return nullptr;
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
