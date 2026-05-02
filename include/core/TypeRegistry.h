//
// Created by Tony Horrobin on 20/04/2026.
//

#pragma once

#include "TypeRegistry.h"
#include "util/VectorMap.h"
#include "util/Searchable.h"

namespace dagbase
{
    struct Type;

    struct DAGBASE_API Field
    {
        Type* type{nullptr};
    };

    struct DAGBASE_API Method
    {
        Type* returnType{nullptr};
        std::vector<Type> arguments;
    };

    using Getter = dagbase::Variant (*)(void *);
    using Setter = void (*)(void *, dagbase::Variant);

    struct DAGBASE_API Property
    {
        Type* type{nullptr};
        Getter getter{nullptr};
        Setter setter{nullptr};

        Variant find(std::string_view path) const
        {
            Variant retval;

            retval = findInternal(path, "type", *type);
            if (retval.has_value())
                return retval;

            return {};
        }
    };

    struct TypeData
    {
        enum MemberType : std::uint32_t
        {
            MEMBER_FIELD,
            MEMBER_METHOD,
            MEMBER_PROPERTY
        };
        std::variant<Field, Method, Property> value;
    };

    struct DAGBASE_API Member
    {
        dagbase::Atom name;
        TypeData data;

        Variant find(std::string_view path) const
        {
            Variant retval;

            retval = findInternal(path, "prop", std::get<Property>(data.value));
            if (retval.has_value())
                return retval;

            return {};
        }
    };

    struct DAGBASE_API Type
    {
        Atom name;
        using Values = std::vector<std::pair<dagbase::Atom, std::uint32_t>>;
        Values values;
        using MemberArray = std::vector<Member>;
        MemberArray members;
        std::size_t size{0};
        bool complete{false};

        std::pair<decltype(members)::const_iterator, decltype(members)::const_iterator> enumerate(std::size_t index) const
        {
            auto pred = [index](const dagbase::Member& op) {
                return op.data.value.index()==index;
            };
            decltype(dagbase::Type::members)::const_iterator itBegin = std::find_if(members.begin(), members.end(), pred);
            decltype(dagbase::Type::members)::const_iterator itEnd=std::find_if_not<std::vector<dagbase::Member>::const_iterator, decltype(pred)>(itBegin, members.end(), pred);

            return std::make_pair(itBegin, itEnd);
        }

        Variant find(std::string_view path) const
        {
            Variant retval;

            for (auto member : members)
            {
                retval = findInternal(path, member.name.value(), member);
                if (retval.has_value())
                    return retval;
            }
            retval = findEndpoint(path, "size", std::uint32_t(size));
            if (retval.has_value())
                return retval;

            for (auto p : values)
            {
                retval = findEndpoint(path, p.first.toString().c_str(), Variant(p.second));
                if (retval.has_value())
                    return retval;
            }
            return {};
        }
    };

    template<typename Enum>
    struct Enumeration : Type
    {
        using ToStringFunc = const char* (*)(Enum);
        using ParseFunc = Enum(*)(const char*);
        Type* base{nullptr};
        ToStringFunc toString{ nullptr };
        ParseFunc parse{ nullptr };
    };

#define DAGBASE_BEGIN_COMPOUND(name)                                                                    \
static dagbase::Type type{};                                                                            \
static bool inited{false};                                                                              \
if (!inited)                                                                                            \
{                                                                                                       \
type.size = sizeof(name);

#define DAGBASE_ADD_FIELD(memberName, typeName)                                                         \
{                                                                                                       \
    dagbase::Member member;                                                                             \
    member.name = dagbase::Atom::intern(#memberName);                                                   \
    member.data.value = dagbase::Field();                                                               \
    std::get<dagbase::Field>(member.data.value).type = &typeName::getType();                            \
    type.members.emplace_back(member);                                                                  \
}

#define DAGBASE_DEFINE_PROPERTY(className, typeName, propName, setPropName)                             \
public:                                                                                                 \
inline static dagbase::Variant propName##_get(void *obj)                                                \
{                                                                                                       \
    auto self = static_cast<className*>(obj);                                                           \
                                                                                                        \
    return dagbase::Variant(self->propName());                                                          \
}                                                                                                       \
                                                                                                        \
inline static void propName##_set(void *obj, dagbase::Variant value)                                    \
{                                                                                                       \
    auto self = static_cast<className*>(obj);                                                           \
                                                                                                        \
    self->setPropName(value.typeName());                                                                \
}

#define DAGBASE_ADD_PROPERTY(className, memberName, typeName)                                           \
    dagbase::Member member;                                                                             \
    member.name = dagbase::Atom::intern(#memberName);                                                   \
    member.data.value = dagbase::Property();                                                            \
    std::get<dagbase::Property>(member.data.value).type = &typeName::getType();                         \
    std::get<dagbase::Property>(member.data.value).getter = className::memberName##_get;                \
    std::get<dagbase::Property>(member.data.value).setter = className::memberName##_set;                \
    type.members.emplace_back(member);

#define DAGBASE_END_COMPOUND()                                                                \
    type.complete = true;                                                                               \
        std::sort(type.members.begin(), type.members.end(), [](const dagbase::Member& op1, const dagbase::Member& op2) {  \
            return op1.data.value.index() < op2.data.value.index();                                                       \
        });                                                                                             \
        inited = true;                                                                                  \
    }


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