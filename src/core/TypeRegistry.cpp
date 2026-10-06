//
// Created by Tony Horrobin on 20/04/2026.
//

#include "core/TypeRegistry.h"
#include "util/enums.h"

#include <cstdint>
#include <string>

namespace dagbase
{
    std::string Type::flagsToString(Flags value)
    {
        std::string retval;

        if (value == FLAGS_NONE)
            return "FLAGS_NONE";

        BIT_NAME(value, BITMASK_BIT, retval)

        if (!retval.empty() && retval.back() == ' ')
            retval.pop_back();

        return retval;
    }

    Type::Flags Type::parseFlags(const std::string& str)
    {
        Flags mask{FLAGS_NONE};

        TEST_BIT(BITMASK_BIT, str, mask);

        return mask;
    }

    std::uint32_t TypeRegistry::registerType(const Atom& name, Type* type)
    {
        if (!name.empty() && type)
        {
            auto p = _types.emplace(name, type);
            type->id = _nextTypeId++;

            return type->id;
        }

        return 0;
    }

    void TypeRegistry::unregisterType(Atom name)
    {
        if (auto it=_types.find(name); it!=_types.end())
            _types.erase(it);
    }

    Type * TypeRegistry::findType(Atom name)
    {
        if (auto it=_types.find(name); it!=_types.end())
            return it->second;

        return nullptr;
    }
}
