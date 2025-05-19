//
// Created by Tony Horrobin on 06/05/2025.
//

#include "config/config.h"

#include "core/IdentifierGenerator.h"

namespace dagbase
{
    IdentifierGenerator::Identifier IdentifierGenerator::generate()
    {
        if (!_recycledIds.empty())
        {
            Identifier id = _recycledIds.front();
            _recycledIds.pop();
            return id;
        }
        return _nextId++;
    }

    void IdentifierGenerator::release(IdentifierGenerator::Identifier id)
    {
        if (id == _nextId-1)
        {
            --_nextId;
        }
        else
        {
            _recycledIds.push(id);
        }
    }
}
