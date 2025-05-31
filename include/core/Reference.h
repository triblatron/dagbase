//
// Created by Tony Horrobin on 18/05/2025.
//

#pragma once

#include "config/DagBaseExport.h"

#include <variant>
#include <string>

namespace dagbase
{
    //! Avoid Windows linker barfage by not specifying DAGBASE_API here.
    template<typename Identifier, typename Ref>
    class Reference
    {
    public:
        Reference() = default;

        explicit Reference(Identifier id)
        :
        _ref(id)
        {
            // Do nothing.
        }

        void setId(Identifier id)
        {
            _ref = id;
        }

        void resolve(Ref* lookup)
        {
            _ref = lookup->lookup(std::get<Identifier>(_ref));
        }

        Ref* ref()
        {
            if (_ref.index()==1)
            {
                return std::get<1>(_ref);
            }

            return nullptr;
        }

        const Ref* ref() const
        {
            if (_ref.index()==1)
            {
                return std::get<1>(_ref);
            }

            return nullptr;
        }

        std::string toString() const
        {
            switch (_ref.index())
            {
                case 0:
                {
                    if (auto value=std::get<0>(_ref); value.value()!=nullptr)
                    {
                        return value.value();
                    }
                }
                    break;
                case 1:
                    return "0x" + std::to_string(reinterpret_cast<std::uintptr_t>(std::get<1>(_ref)));
            }

            return {};
        }
    private:
        using ValueType = std::variant<Identifier, Ref*>;
        ValueType _ref;
    };
}
