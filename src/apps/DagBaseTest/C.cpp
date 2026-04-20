//
// Created by Tony Horrobin on 20/04/2026.
//

#include "C.h"
#include "B.h"

dagbase::Type & C::getType(bool complete)
{
    static dagbase::Type type{};
    static bool inited = false;

    if (!complete)
        return type;

    if (!inited)
    {
        type.size = sizeof(C);
        type.members.emplace_back();
        type.members[0].name = dagbase::Atom::intern("b");
        type.members[0].data = dagbase::Field();
        std::get<0>(type.members[0].data).type = &B::getType(false);
        type.complete = true;
        inited = true;
    }

    return type;
}

dagbase::MetaClassRegistration<C> C::registration(dagbase::Atom::intern("C"));
