//
// Created by Tony Horrobin on 18/12/2025.
//

#include "config/config.h"

#include "core/HierarchicalStateMachine.h"
#include "core/ConfigurationElement.h"
#include "util/Searchable.h"

namespace dagbase
{
    void HierarchicalState::configure(ConfigurationElement &config)
    {
        ConfigurationElement::readConfig(config, "value", &_value);
        if (auto element = config.findElement("children"); element)
        {
            element->eachChild([this](ConfigurationElement& child) {
                auto* state = new HierarchicalState();

                state->configure(child);
                Atom name;
                ConfigurationElement::readConfig(child, "name", &name);
                _children.m.emplace(name, state);
                return true;
            });
        }
    }

    Variant HierarchicalState::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "value", _value);
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "numStates", std::uint32_t(_children.size()));
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "states", _children);
        if (retval.has_value())
            return retval;

        return {};
    }

    HierarchicalState::~HierarchicalState()
    {
        for (auto& p : _children)
        {
            delete p.second;
        }
    }

    void HierarchicalStateMachine::configure(ConfigurationElement& config)
    {
        if (auto element=config.findElement("states"); element)
        {
            element->eachChild([this](ConfigurationElement& child) {
                HierarchicalState* state = nullptr;
                state = new HierarchicalState();
                state->configure(child);
                Atom name;
                ConfigurationElement::readConfig(child, "name", &name);
                _states.emplace(name, state);
                return true;
            });
        }
        if (auto it=_states.m.find(Atom::intern("Initial")); it!=_states.end())
        {
            _currentState = it->second;
        }
    }

    Variant HierarchicalStateMachine::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "numStates", std::uint32_t(_states.m.size()));
        if (retval.has_value())
            return retval;

        if (_currentState)
        {
            retval = findEndpoint(path, "currentState", _currentState->value());
            if (retval.has_value())
                return retval;
        }

        retval = findInternal(path, "states", _states);
        if (retval.has_value())
            return retval;

        return {};
    }

    HierarchicalStateMachine::~HierarchicalStateMachine()
    {
        for (auto& p : _states)
        {
            delete p.second;
        }
    }
}
