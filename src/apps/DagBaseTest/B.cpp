//
// Created by Tony Horrobin on 20/04/2026.
//

#include "B.h"
#include "C.h"

dagbase::Type& B::getType(bool complete)
{
    static dagbase::Type type{};
    static bool inited = false;

    if (!complete)
        return type;

    if (!inited)
    {
        type.size = sizeof(B);
        type.members.emplace_back();
        type.members[0].name = dagbase::Atom::intern("c");
        type.members[0].data = dagbase::Field();
        std::get<0>(type.members[0].data).type = &C::getType(false);
        type.complete = true;
        inited = true;
    }

    return type;
}

MetaClassRegistration<B> B::registration;
