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
    template<typename BaseClass>
    class DynamicVisitor
    {
    public:
        using Handler=std::function<void(BaseClass&)>;
        using HandlerMap=VectorMap<Atom,Handler>;
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
