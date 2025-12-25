//
// Created by Tony Horrobin on 18/12/2025.
//

#include "config/config.h"

#include "core/HierarchicalStateMachine.h"

#include "../../../../../../../../../../../opt/homebrew/include/glm/detail/func_geometric.inl"
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

        if (auto element=config.findElement("regions"); element)
        {
            element->eachChild([this](ConfigurationElement& child) {
                HierarchicalStateMachine* state = nullptr;
                state = new HierarchicalStateMachine();
                state->configure(child);
                Atom name;
                ConfigurationElement::readConfig(child, "name", &name);
                _regions.emplace(name, state);
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
        _currentState = {Atom(),nullptr};
        if (_initialState.empty())
            _initialState = Atom::intern("Initial");
        if (!_initialState.empty())
            setState(findInitialState());
    }

    HierarchicalStateMachine::ChildArray::value_type HierarchicalStateMachine::state()
    {
        if (_currentState.second && _currentState.second->isFlagSet(FLAGS_HAS_VALUE))
            return _currentState;

        for (auto p : _regions)
        {
            auto candidate = p.second->state();

            if (candidate.second)
                return candidate;
        }
        for (auto p : _children)
        {
            if (!p.second->isFlagSet(FLAGS_HAS_VALUE))
            {
                return p.second->state();
            }
        }

        return _currentState;
    }

    HierarchicalStateMachine::ChildArray::iterator HierarchicalStateMachine::parseState(const Atom &name)
    {
        return _children.m.find(name);
    }

    bool HierarchicalStateMachine::onInput(const Atom &input)
    {
        bool handled = false;
        for (auto p : _regions)
        {
            if (!handled)
                handled = p.second->onInput(input);
            else
                return true;
        }
        for (auto p : _children)
        {
            if (!handled)
                handled = p.second->onInput(input);
            else
                return true;
        }
        if (!handled && !isFlagSet(FLAGS_HAS_VALUE) && _currentState.second)
        {
            HierarchicalTransition::Domain domain;
            domain.initialState = _currentState.first;
            domain.input = input;
            if (auto it= _transitionFunction.find(domain); it!=_transitionFunction.end())
            {
                setState(it->second.nextState);
                handled = true;
            }
        }

        return handled;
    }

    Variant HierarchicalStateMachine::find(std::string_view path) const
    {
        Variant retval;

        retval = findEndpoint(path, "numStates", std::uint32_t(_children.m.size()));
        if (retval.has_value())
            return retval;

        if (_currentState.second)
        {
            retval = findEndpoint(path, "currentState", _currentState.second->value());
            if (retval.has_value())
                return retval;
        }

        retval = findEndpoint(path, "numInputs", std::uint32_t(_inputs.size()));
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "numTransitions", std::uint32_t(_transitionFunction.size()));
        if (retval.has_value())
            return retval;

        retval = findEndpoint(path, "numRegions", std::uint32_t(_regions.size()));
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "states", _children);
        if (retval.has_value())
            return retval;

        retval = findInternal(path, "regions", _regions);
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

    Atom HierarchicalStateMachine::findInitialState()
    {
        if (auto it=_children.m.find(_initialState); it!=_children.m.end())
        {
            return it->first;
        }
        return {};
    }

    void HierarchicalStateMachine::addAction(std::string_view path, Atom stateName,
        std::function<int(HierarchicalStateMachine &)>&& f)
    {
        auto dotPos = path.find('.');
        EntryExitActions* a{nullptr};
        HierarchicalStateMachine* self = this;
        ChildArray* children = &_children;
        auto token = path.substr(0,dotPos);
        TransitionActions* transitionActions = nullptr;
        TransitionActions::key_type p;
        if (token == "entryActions")
        {
            a = &_entryActions;
        }
        else if (token == "exitActions")
        {
            a = &_exitActions;
        }
        else if (token == "transitionActions")
        {
            transitionActions = &_transitionActions;
        }
        std::string_view rest = path;
        while (!rest.empty())
        {
            token = rest.substr(0, dotPos);
            if (dotPos!=std::string_view::npos)
                rest = rest.substr(dotPos + 1);
            else
                rest = "";
            if (token == "regions")
            {
                children = &self->_regions;
            }
            else if (token == "entryActions" && self)
            {
                a = &self->_entryActions;
            }
            else if (token == "exitActions" && self)
            {
                a = &self->_exitActions;
            }
            else if (token == "transitionActions" && self)
            {
                transitionActions = &self->_transitionActions;
            }
            else if (!transitionActions)
            {
                std::string strToken = std::string(token);
                self = children->lookup(Atom::intern(strToken));
            }
            dotPos = rest.find('.');
        }
        if (a)
        {
            auto action = new HierarchicalEntryExitAction;
            action->signal.connect(std::forward<std::function<int(HierarchicalStateMachine&)>>(f));
            a->emplace(stateName, action);
        }
        else if (transitionActions)
        {
            auto action = new HierarchicalEntryExitAction;
            dotPos = stateName.find('.');
            if (dotPos!=Atom::npos)
            {
                p.first = stateName.substr(0, dotPos);
                p.second = stateName.substr(dotPos + 1);
            }
            action->signal.connect(std::forward<std::function<int(HierarchicalStateMachine&)>>(f));
            transitionActions->emplace(p, action);
        }
    }

    void HierarchicalEntryExitAction::operator()(HierarchicalStateMachine &state)
    {
        numCalls+=signal(state);
    }

    HierarchicalStateMachine::~HierarchicalStateMachine()
    {
        for (auto& p : _children)
        {
            delete p.second;
        }
        for (auto& p : _regions)
        {
            delete p.second;
        }
        for (auto& p : _entryActions)
        {
            delete p.second;
        }
        for (auto& p : _exitActions)
            delete p.second;
        for (auto& p : _transitionActions)
            delete p.second;
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
            ConfigurationElement &config, const char *name, TransitionActions *value)
    {
        if (value)
            if (auto element=config.findElement(name); element)
            {
                value->reserve(element->numChildren());
                element->eachChild([this](ConfigurationElement& child) {
                    Atom fromState;
                    Atom toState;
                    ConfigurationElement::readConfig(child, "fromState", &fromState);
                    ConfigurationElement::readConfig(child, "toState", &toState);
                    auto action = new HierarchicalEntryExitAction;
                    if (auto actionElement=child.findElement("action"); actionElement)
                    {
                        action->configure(*actionElement);
                    }

                    _transitionActions.emplace(std::make_pair(fromState, toState), action);

                    return true;
                });
            }
    }

    void HierarchicalStateMachine::setState(const Atom& state)
    {
        if (!_currentState.first.empty() && _currentState.second)
            if (auto itAction=_exitActions.m.find(_currentState.first); itAction != _exitActions.m.end())
            {
                (*itAction->second)(*_currentState.second);
            }
        auto nextState = parseState(state);
        if (!_currentState.first.empty() && _currentState.second)
            if (auto itAction=_transitionActions.m.find(std::make_pair(_currentState.first, state)); itAction!=_transitionActions.m.end())
            {
                (*itAction->second)(*_currentState.second);
            }
        if (nextState!=_children.end())
            _currentState = *nextState;
        if (!_currentState.first.empty() && _currentState.second)
            if (auto itAction=_entryActions.m.find(_currentState.first); itAction != _entryActions.m.end())
            {
                (*itAction->second)(*_currentState.second);
            }
    }


    void
    HierarchicalStateMachine::readEntryExitActions(ConfigurationElement &config,
                                                                                  const char *name,
                                                                                  EntryExitActions *value)
    {
        if (value)
            if (auto element=config.findElement(name); element)
            {
                value->reserve(element->numChildren());
                element->eachChild([&value](ConfigurationElement& child) {
                    Atom state;
                    auto action = new HierarchicalEntryExitAction();
                    ConfigurationElement::readConfig(child, "state", &state);

                    if (auto actionElement=child.findElement("action"); actionElement)
                    {
                        action->configure(*actionElement);
                    }

                    value->emplace(state, action);

                    return true;
                });
            }
    }

}
