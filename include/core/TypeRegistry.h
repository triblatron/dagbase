//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "util/VectorMap.h"

namespace dagbase
{
    struct Type;

    struct Field
    {
        Type* type{nullptr};
    };

    struct Method
    {
        std::uint32_t arity{0};
        Type* returnType{nullptr};
        std::vector<Type> arguments;
    };

    struct Member
    {
        dagbase::Atom name;
        std::variant<Field, Method> data;
    };

    struct Type
    {
        std::vector<Member> members;
        std::size_t size{0};
        bool complete{false};
    };

    class TypeRegistry
    {
    public:
        ~TypeRegistry() = default;

        static TypeRegistry& getTypeRegistry()
        {
            static TypeRegistry registry;

            return registry;
        }

        void registerType(dagbase::Atom name, Type* type);

        void unregisterType(dagbase::Atom name);

        Type* findType(dagbase::Atom name);
    private:
        dagbase::VectorMap<dagbase::Atom, Type*> _types;
    };


}