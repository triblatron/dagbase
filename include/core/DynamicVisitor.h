//
// Created by Tony Horrobin on 17/04/2025.
//

#pragma once

#include "core/Atom.h"
#include "util/VectorMap.h"

#include <functional>
#include <map>

namespace dagbase
{
    template<typename Value>
    struct WildcardLess
    {
        bool operator()(const std::pair<dagbase::Atom, Value>& op1, const std::pair<dagbase::Atom, Value>& op2) const
        {
            if (op1.first == "*" || op2.first == "*")
                return false;

            return op1.first < op2.first;
        }
    };

    struct WildcardEquals
    {
        bool operator()(const dagbase::Atom& op1, const dagbase::Atom& op2) const
        {
            if (op1 == "*" || op2 == "*")
                return true;

            return op1 == op2;
        }
    };

    template<typename BaseClass>
    class DynamicVisitor
    {
    public:
        using Handler=std::function<void(BaseClass&)>;
        using HandlerMap=VectorMap<Atom,Handler,WildcardLess<Handler>, WildcardEquals>;
    public:
        void visit(BaseClass& obj)
        {
            Handler handler = findHandler(obj.typeName());

            if (handler)
            {
                handler(obj);
            }
        }

        Handler findHandler(Atom name)
        {
            if (auto it=_handlers.find(name); it!=_handlers.end())
            {
                return it->second;
            }
            return {};
        }

        void registerHandler(Atom name, Handler handler)
        {
            _handlers.insert(std::make_pair(name,handler));
        }
    private:
        HandlerMap _handlers;
    };
}
