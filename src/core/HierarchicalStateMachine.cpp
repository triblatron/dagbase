//
// Created by Tony Horrobin on 18/12/2025.
//

#include "config/config.h"

#include "core/HierarchicalStateMachine.h"
#include "core/ConfigurationElement.h"
#include "util/Searchable.h"

namespace dagbase
{
    void SimpleState::configure(ConfigurationElement &config)
    {
        ConfigurationElement::readConfig(config, "value", &_value);
    }

    Variant SimpleState::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "value", _value);
        if (retval.has_value())
            return retval;

        return {};
    }

    void HierarchicalStateMachine::configure(ConfigurationElement& config)
    {
        SimpleState::configure(config);
        if (auto element=config.findElement("states"); element)
        {
            element->eachChild([this](ConfigurationElement& child) {
                SimpleState* state = nullptr;
                if (auto statesElement=child.findElement("states"); statesElement)
                {
                    state = new HierarchicalStateMachine();
                }
                else
                {
                    state = new SimpleState();
                }
                state->configure(child);
                Atom name;
                ConfigurationElement::readConfig(child, "name", &name);
                _states.emplace(name, state);
                return true;
            });
        }
    }

    Variant HierarchicalStateMachine::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "numStates", std::uint32_t(_states.m.size()));
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "states", _states);
        if (retval.has_value())
            return retval;

        return {};
    }

    HierarchicalStateMachine::~HierarchicalStateMachine()
    {
        for (auto p : _states)
        {
            delete p.second;
        }
    }
}
