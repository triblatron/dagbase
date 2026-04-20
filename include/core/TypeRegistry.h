//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "util/VectorMap.h"

namespace dagbase
{
    struct Type;

    struct DAGBASE_API Field
    {
        Type* type{nullptr};
    };

    struct DAGBASE_API Method
    {
        std::uint32_t arity{0};
        Type* returnType{nullptr};
        std::vector<Type> arguments;
    };

    struct DAGBASE_API Member
    {
        dagbase::Atom name;
        std::variant<Field, Method> data;
    };

    struct DAGBASE_API Type
    {
        std::vector<Member> members;
        std::size_t size{0};
        bool complete{false};
    };

    template<typename Enum>
    struct Enumeration : Type
    {
        using ToStringFunc = const char* (*)(Enum);
        using ParseFunc = Enum(*)(const char*);
        ToStringFunc toString{ nullptr };
        ParseFunc parse{ nullptr };
    };

    class DAGBASE_API TypeRegistry
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