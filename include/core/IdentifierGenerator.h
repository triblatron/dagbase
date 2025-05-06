//
// Created by Tony Horrobin on 06/05/2025.
//

#include "config/DagBaseExport.h"

#include <cstdint>
#include <vector>
#include <queue>

namespace dagbase
{
    class DAGBASE_API IdentifierGenerator
    {
    public:
        using Identifier=std::uint32_t;
        using RecycleQueue=std::queue<Identifier>;
    public:
        Identifier generate();

        void release(Identifier id);
    private:
        Identifier _nextId{0};
        RecycleQueue _recycledIds;
    };
}