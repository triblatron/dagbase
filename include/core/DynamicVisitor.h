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
        using HandlerMap=std::map<std::string,Handler>;
    public:
        void visit(BaseClass& obj)
        {
            Handler handler = findHandler(obj.typeName());

            if (handler)
            {
                handler(obj);
            }
        }

        Handler findHandler(std::string name)
        {
            if (auto it=_handlers.find(name); it!=_handlers.end())
            {
                return it->second;
            }
            return {};
        }
        void registerHandler(std::string name, Handler handler)
        {
            _handlers.insert(std::make_pair(name,handler));
            //_ints.insert(std::make_pair(1,handler));
        }
    private:
        HandlerMap _handlers;
        std::map<int,int> _ints;
    };
}
