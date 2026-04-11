//
// Created by Tony Horrobin on 06/04/2026.
//

#pragma once

#include "config/DagBaseExport.h"

#include "core/Atom.h"
#include "core/Class.h"

#include <cstring>

namespace dagbase
{
    class DAGBASE_API PropertyBase
    {
    public:
        explicit PropertyBase(const Atom& name)
            :
        _name(name)
        {
            // Do nothing.
        }

        virtual ~PropertyBase() = default;

        const Atom& name() const
        {
            return _name;
        }

        virtual void get(const Class* obj, void* buffer, std::size_t bufferSize) const = 0;
    private:
        Atom _name;
    };

    template<typename Field, typename Owner>
    class Property : public PropertyBase
    {
    public:
        using Getter = void (Owner::*)(Field* buffer) const;
        using Setter = void(Owner::*)(const Field&);
        using Notify = void(Owner::*)();
    public:
        Property(const Atom& name, Getter getter, Setter setter, Notify notify)
            :
        PropertyBase(name),
        _getter(getter),
        _setter(setter),
        _notify(notify)
        {
            // Do nothing.
        }

        void get(const Class* obj, void* buffer, std::size_t bufferSize) const override
        {
            if (_getter)
            {
                const auto* owner = dynamic_cast<const Owner*>(obj);
                if (bufferSize>=sizeof(Field))
                {
                    get(owner, static_cast<Field*>(buffer));
                }
            }
        }

        void get(const Owner* obj, Field* buffer) const
        {
            if (_getter)
            {
                (obj->*_getter)(buffer);
            }
        }

        void set(Owner& obj, const Field& field)
        {
            if (_setter)
                (obj.*_setter)(field);
        }

        void notify(Owner& obj)
        {
            if (_notify)
                (obj.*_notify)();
        }
    private:
        Getter _getter{nullptr};
        Setter _setter{nullptr};
        Notify _notify{nullptr};
    };

}