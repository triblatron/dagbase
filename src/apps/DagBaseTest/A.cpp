//
// Created by Tony Horrobin on 20/04/2026.
//

#include "A.h"

dagbase::Type & A::getType()
{
    // Use the Meyer Singleton pattern to create a static object in a thread-safe manner.
    static dagbase::Type type{};
    static bool inited = false;
    if (!inited)
    {
        std::cout << "MetaClassRegistration<A> registering" << std::endl;
        type.size = sizeof(A);
        type.members.emplace_back();
        type.members[0].name = dagbase::Atom::intern("parent");
        type.members[0].data.value = dagbase::Field();
        std::get<0>(type.members[0].data.value).type = &type;
        type.complete = true;
        inited = true;
    }

    return type;
}

dagbase::MetaClassRegistration<A> A::registration(dagbase::Atom::intern("A"));